import dis
import json

import io
import re

import numba
import numba.types as types
from numba import typeof

import numpy as np

from pandas import DataFrame

from jitq.ast_optimizer import OPT_CONST_PROPAGATE, ast_optimize
from jitq.benchmarks.timer import Timer
from jitq.c_executor import Executor
from jitq.config import DUMP_DAG, FAST_MATH
from jitq.constant_strings import ID, PREDS, DAG, OP, MAP, FUNC, \
    OUTPUT_TYPE, FILTER, FLAT_MAP, REDUCE, REDUCEBYKEY, DATA_PATH, ADD_INDEX, \
    FROM, TO, STEP
from jitq.utils import replace_unituple, get_project_path, RDDEncoder, \
    make_tuple, flatten, numba_type_to_dtype
from jitq.libs.numba.llvm_ir import cfunc


def clean_rdds(rdd):
    rdd.dic = None
    for par in rdd.parents:
        clean_rdds(par)


def numba_abi_to_llvm_abi(type_):
    if isinstance(type_, types.List):
        inner_type = type_.dtype
        out = types.List(numba_abi_to_llvm_abi(inner_type), reflected=False)
    elif isinstance(type_, types.Tuple):
        type_type = type(type_)
        child_types = []
        for child_type in type_.types:
            child_types.append(numba_abi_to_llvm_abi(child_type))
        out = type_type(child_types)
    elif isinstance(type_, types.UniTuple):
        type_type = type(type_)
        out = type_type(numba_abi_to_llvm_abi(type_.dtype), type_.count)
    else:
        out = type_
    return out


def get_llvm_ir_and_output_type(func, input_type=None, opts=None):
    opts_ = {OPT_CONST_PROPAGATE}
    if opts is not None:
        opts_ += opts
    func = ast_optimize(func, opts_)
    input_type = replace_unituple(input_type)
    timer = Timer()
    timer.start()
    if isinstance(input_type, list):
        dec_func = numba.njit(
            tuple(input_type), fastmath=FAST_MATH, parallel=True)(func)
    else:
        dec_func = numba.njit(
            (input_type, ), fastmath=FAST_MATH, parallel=True)(func)
    timer.end()
    output_type = dec_func.nopython_signatures[0].return_type

    output_type = replace_unituple(output_type)

    input_type = dec_func.nopython_signatures[0].args

    cfunc_code = cfunc(output_type(*input_type), fastmath=FAST_MATH)(func)
    code = cfunc_code.inspect_llvm()
    # Extract just the code of the function
    # pylint: disable=no-member
    # sabir 14.02.18: somehow pylint does not understand S = DOTALL = ... in re
    code_group = re.search('define [^\n\r]* @"cfunc.*', code, re.DOTALL)
    code_string = code_group.group(0)
    code_string = re.sub("attributes.*}", "", code_string)

    # make the function name not unique
    code_string = re.sub("@.*\".*\"", "@cfuncnotuniquename", code_string)

    return code_string, output_type


def get_llvm_ir_for_generator(func):
    dec_func = numba.njit(())(func)
    output_type = dec_func.nopython_signatures[0].return_type
    llvm = dec_func.inspect_llvm()[()]
    output_type = output_type.yield_type
    return llvm, output_type


class RDD(object):
    """
    Dataset representation.
    RDDs represent dataflow operators.

    """

    def __init__(self, context, parents):
        self.dic = None
        self._cache = False
        self.parents = parents
        self.context = context
        self.output_type = None
        self.hash = None
        if any([p.context != self.context for p in self.parents]):
            raise ValueError("The context of all parents must be the same!")

    def cache(self):
        self._cache = True
        return self

    def write_dag(self, daglist, index, empty=False):
        if self.dic:
            return self.dic[ID]
        cur_index = index
        preds_index = []
        for par in self.parents:
            pred_index = par.write_dag(daglist, cur_index)
            cur_index = max(pred_index + int(not empty), cur_index)
            preds_index.append(pred_index)

        if not empty:
            dic = dict()
            dic[ID] = cur_index
            dic[PREDS] = preds_index
            self.dic = dic
            daglist.append(dic)
        return cur_index

    def execute_dag(self):
        hash_ = str(hash(self))
        dag_dict = self.context.serialization_cache.get(hash_, None)
        inputs = self.get_inputs()
        if not dag_dict:
            dag_dict = dict()
            dag_dict[DAG] = []

            clean_rdds(self)

            self.write_dag(dag_dict[DAG], 0)

            self.context.serialization_cache[hash_] = dag_dict
            # write to file
            if DUMP_DAG:
                with open(get_project_path() + '/dag.json', 'w') as fp:
                    json.dump(dag_dict, fp=fp, cls=RDDEncoder)
        return Executor().execute(self.context, dag_dict, inputs)

    def __hash__(self):
        if not self.hash:
            parent_hashes = "#".join([str(hash(p)) for p in self.parents])
            self_hash = str(self.self_hash())
            self.hash = hash(type(self).__name__ + parent_hashes + self_hash)
        return self.hash

    # pylint: disable=no-self-use
    def self_hash(self):
        return hash("")

    def get_inputs(self):
        ret = []
        for parent in self.parents:
            ret += parent.get_inputs()
        return ret

    def map(self, map_func):
        return Map(self.context, self, map_func)

    def flatten(self):
        return Flatten(self.context, self)

    def filter(self, predicate):
        return Filter(self.context, self, predicate)

    def flat_map(self, func):
        return FlatMap(self.context, self, func)

    def reduce_by_key(self, func):
        return ReduceByKey(self.context, self, func)

    def reduce(self, func):
        return Reduce(self.context, self, func).collect()[0]

    def join(self, other):
        return Join(self.context, self, other)

    def cartesian(self, other):
        return Cartesian(self.context, self, other)

    def collect(self):
        return self.execute_dag()

    def count(self):
        return self.flatten().map(lambda t: 1).reduce(lambda t1, t2: t1 + t2)


class SourceRDD(RDD):
    def __init__(self, context):
        super(SourceRDD, self).__init__(context, [])


class UnaryRDD(RDD):
    def __init__(self, context, parent):
        super(UnaryRDD, self).__init__(context, [parent])


class BinaryRDD(RDD):
    def __init__(self, context, parents):
        super(BinaryRDD, self).__init__(context, parents)
        assert len(parents) == 2


class PipeRDD(UnaryRDD):
    def __init__(self, context, parent, func):
        super(PipeRDD, self).__init__(context, parent)
        self.func = func

    def self_hash(self):
        file_ = io.StringIO()
        dis.dis(self.func, file=file_)
        return hash(file_.getvalue())


class Map(PipeRDD):
    def write_dag(self, daglist, index, empty=False):
        cur_index = super(Map, self).write_dag(daglist, index)
        dic = self.dic
        dic[OP] = MAP
        dic[FUNC], self.output_type = get_llvm_ir_and_output_type(
            self.func, self.parents[0].output_type)
        # TODO(ingo): what output types are valid here?
        dic[OUTPUT_TYPE] = make_tuple(flatten(self.output_type))
        return cur_index


class Filter(PipeRDD):
    def write_dag(self, daglist, index, empty=False):
        cur_index = super(Filter, self).write_dag(daglist, index)
        dic = self.dic
        dic[OP] = FILTER
        dic[FUNC], return_type = get_llvm_ir_and_output_type(
            self.func, self.parents[0].output_type)
        if str(return_type) != "bool":
            raise BaseException(
                "Function given to reduce_by_key has the wrong return type:\n"
                "  expected: {0}\n"
                "  found:    {1}".format("bool", return_type))
        self.output_type = self.parents[0].output_type
        dic[OUTPUT_TYPE] = make_tuple(flatten(self.output_type))
        return cur_index


class FlatMap(PipeRDD):
    def write_dag(self, daglist, index, empty=False):
        if self.dic:
            return self.dic[ID]
        cur_index = super(FlatMap, self).write_dag(daglist, index)
        dic = self.dic
        dic[OP] = FLAT_MAP
        dic[FUNC], self.output_type = get_llvm_ir_for_generator(self.func)
        dic[OUTPUT_TYPE] = make_tuple(flatten(self.output_type))
        return cur_index


class Flatten(UnaryRDD):
    def write_dag(self, daglist, index, empty=False):
        cur_index = super(Flatten, self).write_dag(daglist, index, True)
        self.output_type = make_tuple(flatten(self.parents[0].output_type))
        return cur_index


class Join(BinaryRDD):
    """
    the first element in a tuple is the key
    """

    def __init__(self, context, left, right):
        super(Join, self).__init__(context, [left, right])

    def compute_output_type(self):
        par_output_type1 = self.parents[0].output_type
        par_output_type2 = self.parents[1].output_type

        if isinstance(par_output_type1, types.BaseTuple):
            key_type = par_output_type1[0]
            output1 = [t for t in par_output_type1.types[1:]]
        else:
            key_type = par_output_type1
            output1 = None

        if isinstance(par_output_type2, types.BaseTuple):
            output2 = [t for t in par_output_type2.types[1:]]
        else:
            output2 = None

        if output1 is None:
            if output2 is None:
                return key_type
            output2.insert(0, key_type)
            return types.Tuple(output2)
        if output2 is None:
            output1.insert(0, key_type)
            return types.Tuple(output1)

        return types.Tuple(
            [key_type,
             types.Tuple(flatten(output1) + flatten(output2))])

    def write_dag(self, daglist, index, empty=False):
        if self.dic:
            return self.dic[ID]
        cur_index = super(Join, self).write_dag(daglist, index)
        dic = self.dic
        dic[OP] = 'join'

        self.output_type = replace_unituple(self.compute_output_type())
        dic[OUTPUT_TYPE] = make_tuple(flatten(self.output_type))
        return cur_index


class Cartesian(BinaryRDD):
    def __init__(self, context, left, right):
        super(Cartesian, self).__init__(context, [left, right])

    def compute_output_type(self):
        parent_type_1 = make_tuple(self.parents[0].output_type) if len(
            self.parents[0].output_type) == 1 else self.parents[0].output_type
        parent_type_2 = make_tuple(self.parents[1].output_type) if len(
            self.parents[1].output_type) == 1 else self.parents[1].output_type
        return [parent_type_1, parent_type_2]

    def write_dag(self, daglist, index, empty=False):
        if self.dic:
            return self.dic[ID]
        cur_index = super(Cartesian, self).write_dag(daglist, index)
        dic = self.dic
        dic[OP] = 'cartesian'

        self.output_type = replace_unituple(self.compute_output_type())
        dic[OUTPUT_TYPE] = make_tuple(flatten(self.output_type))
        return cur_index


class Reduce(UnaryRDD):
    """
    binary function must be commutative and associative
    the return value type should be the same as its arguments
    the input cannot be empty
    """

    def __init__(self, context, parent, func):
        super(Reduce, self).__init__(context, parent)
        self.func = func

    def self_hash(self):
        file_ = io.StringIO()
        dis.dis(self.func, file=file_)
        return hash(file_.getvalue())

    def _compute_input_type(self):
        # repeat the input type two times
        return [self.parents[0].output_type, self.parents[0].output_type]

    def write_dag(self, daglist, index, empty=False):
        cur_index = super(Reduce, self).write_dag(daglist, index)
        dic = self.dic
        dic[OP] = REDUCE
        input_type = self._compute_input_type()
        dic[FUNC], self.output_type = get_llvm_ir_and_output_type(
            self.func, input_type)
        if str(input_type[0]) != str(self.output_type):
            raise BaseException(
                "Function given to reduce_by_key has the wrong return type:\n"
                "  expected: {0}\n"
                "  found:    {1}".format(input_type[0], self.output_type))
        dic[OUTPUT_TYPE] = make_tuple(flatten(self.output_type))
        return cur_index


class ReduceByKey(UnaryRDD):
    """
    binary function must be commutative and associative
    the return value type should be the same as its arguments minus the key
    the input cannot be empty
    """

    def __init__(self, context, parent, func):
        super(ReduceByKey, self).__init__(context, parent)
        self.func = func

    def self_hash(self):
        file_ = io.StringIO()
        dis.dis(self.func, file=file_)
        return hash(file_.getvalue())

    def _compute_input_type(self):
        # repeat the input type two times minus the key
        par_type = self.parents[0].output_type

        def remove_key(type_):
            try:
                if isinstance(type_[0], types.BaseTuple):
                    inner_key_type = remove_key(type_[0])
                    type_ = make_tuple([inner_key_type] + list(type_[1:]))
                else:
                    type_ = make_tuple(type_[1:])
            except TypeError:
                pass
            return type_

        par_type = remove_key(par_type)
        if len(par_type) == 1:
            return [par_type[0], par_type[0]]
        return [par_type, par_type]

    def write_dag(self, daglist, index, empty=False):
        cur_index = super(ReduceByKey, self).write_dag(daglist, index)
        dic = self.dic
        dic[OP] = REDUCEBYKEY
        input_type = self._compute_input_type()
        dic[FUNC], output_type = get_llvm_ir_and_output_type(
            self.func, input_type)
        if str(input_type[0]) != str(output_type):
            raise BaseException(
                "Function given to reduce_by_key has the wrong return type:\n"
                "  expected: {0}\n"
                "  found:    {1}".format(input_type[0], output_type))
        self.output_type = self.parents[0].output_type
        dic[OUTPUT_TYPE] = make_tuple(flatten(self.output_type))
        return cur_index


class CSVSource(SourceRDD):
    def __init__(self, context, path, delimiter=",",
                 dtype=None, add_index=False):
        super(CSVSource, self).__init__(context)
        self.path = path
        self.dtype = dtype
        self.add_index = add_index
        self.delimiter = delimiter

    def self_hash(self):
        hash_objects = [
            self.path,
            str(self.dtype),
            str(self.add_index), self.delimiter,
            self.compute_output()
        ]
        return hash("#".join(hash_objects))

    def compute_output(self):
        df = np.genfromtxt(
            self.path, dtype=self.dtype, delimiter=self.delimiter, max_rows=1)

        try:
            self.output_type = replace_unituple(typeof(tuple(df[0])))
        except TypeError:  # scalar type
            self.output_type = typeof(df[0])

    def write_dag(self, daglist, index, empty=False):
        cur_index = super(CSVSource, self).write_dag(daglist, index)
        self.compute_output()
        dic = self.dic
        dic[OP] = 'csv_source'
        dic[OUTPUT_TYPE] = make_tuple(flatten(self.output_type))
        dic[DATA_PATH] = self.path
        dic[ADD_INDEX] = self.add_index
        return cur_index


class CollectionSource(SourceRDD):
    """
    accepts any python collections, numpy arrays or pandas dataframes

    passed python collection will be copied into a numpy array
    a reference to the array is stored in this instance
    to prevent freeing the input memory before the end of computation
    """

    def __init__(self, context, values, add_index=False):
        super(CollectionSource, self).__init__(context)

        if isinstance(values, DataFrame):
            self.array = values.values.ravel(order='C').reshape(values.shape)
        elif isinstance(values, np.ndarray):
            self.array = values
        else:
            self.output_type = replace_unituple(typeof(values[0]))
            self.array = np.array(
                values, dtype=numba_type_to_dtype(self.output_type))
        assert self.array.size > 0, "Empty collection not allowed"
        try:
            self.output_type = replace_unituple(typeof(tuple(self.array[0])))
        except TypeError:  # scalar type
            self.output_type = typeof(self.array[0])
        if add_index:
            self.output_type = replace_unituple(
                types.Tuple(flatten([numba.typeof(1), self.output_type])))
        self.data_ptr = self.array.__array_interface__['data'][0]
        self.size = self.array.shape[0]
        self.add_index = add_index

    def self_hash(self):
        hash_objects = [str(self.output_type), str(self.add_index)]
        return hash("#".join(hash_objects))

    def write_dag(self, daglist, index, empty=False):
        cur_index = super(CollectionSource, self).write_dag(daglist, index)
        dic = self.dic
        dic[OP] = 'collection_source'
        dic[OUTPUT_TYPE] = make_tuple(flatten(self.output_type))
        dic[ADD_INDEX] = self.add_index

        return cur_index

    def get_inputs(self):
        return [(self.data_ptr, self.size)]


class RangeSource(SourceRDD):
    def __init__(self, context, from_, to, step=1):
        super(RangeSource, self).__init__(context)
        self.from_ = from_
        self.to = to
        self.step = step
        self.output_type = typeof(step + from_)

    def self_hash(self):
        hash_objects = [
            str(o) for o in [self.from_, self.to, self.step, self.output_type]
        ]
        return hash("#".join(hash_objects))

    def write_dag(self, daglist, index, empty=False):
        cur_index = super(RangeSource, self).write_dag(daglist, index)
        dic = self.dic
        dic[OP] = 'range_source'
        dic[FROM] = self.from_
        dic[TO] = self.to
        dic[STEP] = self.step
        dic[OUTPUT_TYPE] = self.output_type
        return cur_index


class GeneratorSource(SourceRDD):
    def __init__(self, context, func):
        super(GeneratorSource, self).__init__(context)
        self.func = func

    def self_hash(self):
        file_ = io.StringIO()
        dis.dis(self.func, file=file_)
        return hash(file_.getvalue())

    def write_dag(self, daglist, index, empty=False):
        cur_index = super(GeneratorSource, self).write_dag(daglist, index)
        dic = self.dic
        dic[OP] = 'generator_source'
        dic[FUNC], self.output_type = get_llvm_ir_for_generator(self.func)
        dic[OUTPUT_TYPE] = self.output_type
        return cur_index
