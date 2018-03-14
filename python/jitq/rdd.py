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
from jitq.c_executor import Executor
from jitq.config import FAST_MATH, DUMP_DAG
from jitq.constant_strings import ID, PREDS, DAG, OP, FUNC, \
    OUTPUT_TYPE, DATA_PATH, ADD_INDEX, FROM, TO, STEP
from jitq.utils import replace_unituple, get_project_path, RDDEncoder, \
    make_tuple, item_typeof, flatten, numba_type_to_dtype, is_item_type, \
    Timer
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


def get_llvm_ir_and_output_type(func, arg_types=None, opts=None):
    opts_ = {OPT_CONST_PROPAGATE}
    if opts is not None:
        opts_ += opts
    if arg_types is None:
        arg_types = []
    func = ast_optimize(func, opts_)

    timer = Timer()
    timer.start()
    dec_func = numba.njit(
        tuple(arg_types), fastmath=FAST_MATH, parallel=True)(func)
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
    NAME = 'abstract'

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

    def write_dag(self, daglist, index):
        # Try to answer from cache
        if self.dic:
            return self.dic[ID]

        # Compute parents
        cur_index = index
        preds_index = []
        for par in self.parents:
            pred_index = par.write_dag(daglist, cur_index)
            cur_index = max(pred_index, cur_index)
            preds_index.append(pred_index)

        # Compute this node
        self.dic = dict()
        empty = self.self_write_dag(self.dic)
        cur_index += int(not empty)
        assert is_item_type(self.output_type), \
            "Expected valid nested tuple type."

        if not empty:
            self.dic[ID] = cur_index
            self.dic[PREDS] = preds_index
            self.dic[OP] = self.NAME
            self.dic[OUTPUT_TYPE] = make_tuple(flatten(self.output_type))
            daglist.append(self.dic)

        return cur_index

    # pylint: disable=no-self-use
    def self_write_dag(self, dic):
        pass

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
    NAME = 'map'

    def self_write_dag(self, dic):
        dic[FUNC], self.output_type = get_llvm_ir_and_output_type(
            self.func, [self.parents[0].output_type])
        if not is_item_type(self.output_type):
            raise BaseException(
                "Function given to map has the wrong return type:\n"
                "  found:    {0}".format(self.output_type))


class Filter(PipeRDD):
    NAME = 'filter'

    def self_write_dag(self, dic):
        dic[FUNC], return_type = get_llvm_ir_and_output_type(
            self.func, [self.parents[0].output_type])
        if str(return_type) != "bool":
            raise BaseException(
                "Function given to reduce_by_key has the wrong return type:\n"
                "  expected: {0}\n"
                "  found:    {1}".format("bool", return_type))
        self.output_type = self.parents[0].output_type


class FlatMap(PipeRDD):
    NAME = 'flat_map'

    def self_write_dag(self, dic):
        dic[FUNC], self.output_type = get_llvm_ir_for_generator(self.func)


class Flatten(UnaryRDD):
    NAME = 'flatten'

    def self_write_dag(self, dic):
        self.output_type = make_tuple(flatten(self.parents[0].output_type))
        return True


class Join(BinaryRDD):
    NAME = 'join'

    """
    the first element in a tuple is the key
    """

    def __init__(self, context, left, right):
        super(Join, self).__init__(context, [left, right])

    def compute_output_type(self):
        left_type = self.parents[0].output_type
        right_type = self.parents[1].output_type
        if not isinstance(left_type, types.Tuple):
            left_type = make_tuple([left_type])
        if not isinstance(right_type, types.Tuple):
            right_type = make_tuple([right_type])

        # Special case: two scalar inputs produce a scalar output
        if not isinstance(self.parents[0].output_type, types.Tuple) and \
           not isinstance(self.parents[1].output_type, types.Tuple):
            return self.parents[0].output_type

        # Common case: concatenate tuples
        key_type = left_type[0]
        left_payload = left_type.types[1:]
        right_payload = right_type.types[1:]

        return make_tuple((key_type,) + left_payload + right_payload)

    def self_write_dag(self, dic):
        self.output_type = self.compute_output_type()


class Cartesian(BinaryRDD):
    NAME = 'cartesian'

    def __init__(self, context, left, right):
        super(Cartesian, self).__init__(context, [left, right])

    def compute_output_type(self):
        left_type = self.parents[0].output_type
        right_type = self.parents[1].output_type
        if not isinstance(left_type, types.Tuple):
            left_type = make_tuple([left_type])
        if not isinstance(right_type, types.Tuple):
            right_type = make_tuple([right_type])
        return make_tuple(left_type.types + right_type.types)

    def self_write_dag(self, dic):
        self.output_type = self.compute_output_type()


class Reduce(UnaryRDD):
    NAME = 'reduce'

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

    def self_write_dag(self, dic):
        aggregate_type = self.parents[0].output_type
        dic[FUNC], self.output_type = get_llvm_ir_and_output_type(
            self.func, [aggregate_type, aggregate_type])
        if str(aggregate_type) != str(self.output_type):
            raise BaseException(
                "Function given to reduce_by_key has the wrong return type:\n"
                "  expected: {0}\n"
                "  found:    {1}".format(aggregate_type, self.output_type))


class ReduceByKey(UnaryRDD):
    NAME = 'reduce_by_key'

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

    def self_write_dag(self, dic):
        aggregate_type = make_tuple(self.parents[0].output_type.types[1:])
        if len(aggregate_type) == 1:
            aggregate_type = aggregate_type.types[0]
        dic[FUNC], output_type = get_llvm_ir_and_output_type(
            self.func, [aggregate_type, aggregate_type])
        if str(aggregate_type) != str(output_type):
            raise BaseException(
                "Function given to reduce_by_key has the wrong return type:\n"
                "  expected: {0}\n"
                "  found:    {1}".format(aggregate_type, output_type))
        self.output_type = self.parents[0].output_type


class CSVSource(SourceRDD):
    NAME = 'csv_source'

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
            self.output_type
        ]
        return hash("#".join(hash_objects))

    def self_write_dag(self, dic):
        df = np.genfromtxt(
            self.path, dtype=self.dtype, delimiter=self.delimiter, max_rows=1)
        self.output_type = item_typeof(df[0])
        dic[DATA_PATH] = self.path
        dic[ADD_INDEX] = self.add_index


class CollectionSource(SourceRDD):
    NAME = 'collection_source'

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
            self.output_type = item_typeof(self.array[0])
        elif isinstance(values, np.ndarray):
            self.array = values
            self.output_type = item_typeof(self.array[0])
        else:
            # Any subscriptable iterator should work here
            self.output_type = item_typeof(values[0])
            self.array = np.array(
                values, dtype=numba_type_to_dtype(self.output_type))
        if add_index:
            if isinstance(self.output_type, types.Tuple):
                child_types = self.output_type.types
            else:
                child_types = (self.output_type,)
            self.output_type = make_tuple((typeof(0),) + child_types)

        assert self.array.size > 0, "Empty collection not allowed"
        self.data_ptr = self.array.__array_interface__['data'][0]
        self.size = self.array.shape[0]
        self.add_index = add_index

    def self_hash(self):
        hash_objects = [str(self.output_type), str(self.add_index)]
        return hash("#".join(hash_objects))

    def self_write_dag(self, dic):
        dic[ADD_INDEX] = self.add_index

    def get_inputs(self):
        return [(self.data_ptr, self.size)]


class RangeSource(SourceRDD):
    NAME = 'range_source'

    def __init__(self, context, from_, to, step=1):
        super(RangeSource, self).__init__(context)
        self.from_ = from_
        self.to = to
        self.step = step
        self.output_type = item_typeof(step + from_)

    def self_hash(self):
        hash_objects = [
            str(o) for o in [self.from_, self.to, self.step, self.output_type]
        ]
        return hash("#".join(hash_objects))

    def self_write_dag(self, dic):
        dic[FROM] = self.from_
        dic[TO] = self.to
        dic[STEP] = self.step


class GeneratorSource(SourceRDD):
    NAME = 'generator_source'

    def __init__(self, context, func):
        super(GeneratorSource, self).__init__(context)
        self.func = func

    def self_hash(self):
        file_ = io.StringIO()
        dis.dis(self.func, file=file_)
        return hash(file_.getvalue())

    def self_write_dag(self, dic):
        dic[FUNC], self.output_type = get_llvm_ir_for_generator(self.func)
