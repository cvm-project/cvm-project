import json
import re

from numpy import ndarray
from pandas import DataFrame

from jitq.ast_optimizer import *
from jitq.benchmarks.timer import Timer
from jitq.c_executor import execute
from jitq.config import DUMP_DAG, FAST_MATH

from jitq.utils import *
from jitq.libs.numba.llvm_ir import cfunc
from jitq.constants import *

import numba
import numba.types as types
from numba.types.containers import *

import dis, io

def cleanRDDs(rdd):
    rdd.dic = None
    for par in rdd.parents:
        cleanRDDs(par)


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
        dec_func = numba.njit(tuple(input_type), fastmath=FAST_MATH, parallel=True)(func)
    else:
        dec_func = numba.njit((input_type,), fastmath=FAST_MATH, parallel=True)(func)
    timer.end()
    output_type = dec_func.nopython_signatures[0].return_type

    output_type = replace_unituple(output_type)

    input_type = dec_func.nopython_signatures[0].args

    cfunc_code = cfunc(output_type(*input_type), fastmath=FAST_MATH)(func)
    code = cfunc_code.inspect_llvm()
    # Extract just the code of the function
    m = re.search('define [^\n\r]* @"cfunc.*', code, re.DOTALL)
    code_string = m.group(0)
    code_string = re.sub("attributes.*}", "", code_string)

    # make the function name not unique
    code_string = re.sub("@.*\".*\"", "@cfuncnotuniquename", code_string)

    return code_string, output_type


def get_llvm_ir_output_type_generator(func):
    dec_func = numba.njit(())(func)
    output_type = dec_func.nopython_signatures[0].return_type
    llvm = dec_func.inspect_llvm()[()]
    output_type = output_type.yield_type
    return llvm, output_type


rdd_dag_cache = {}


class RDD(object):
    def __init__(self, parent=None):
        self.dic = None
        self._cache = False
        if parent:
            self.parents = [parent]
        else:
            self.parents = []
        self.output_type = None
        self.hash = None

    def cache(self):
        self.cache = True
        return self

    def writeDAG(self, daglist, index, empty = False):
        if self.dic:
            return self.dic[ID]
        cur_index = index
        preds_index = []
        for par in self.parents:
            pred_index = par.writeDAG(daglist, cur_index)
            cur_index = max(pred_index + int(not empty), cur_index)
            preds_index.append(pred_index)

        if not empty:
            dic = dict()
            dic[ID] = cur_index
            dic[PREDS] = preds_index
            self.dic = dic
            daglist.append(dic)
        return cur_index

    def executeDAG(self):
        hash_ = str(hash(self))
        dagdict = rdd_dag_cache.get(hash_, None)
        inputs = self.get_inputs()
        if dagdict:
            return execute(dagdict, hash_, inputs)
        else:
            dagdict = dict()
            dagdict[DAG] = []

            cleanRDDs(self)

            self.writeDAG(dagdict[DAG], 0)

            rdd_dag_cache[hash_] = dagdict
            # write to file
            if DUMP_DAG:
                with open(getJitqPath() + '/dag.json', 'w') as fp:
                    json.dump(dagdict, fp=fp, cls=RDDEncoder)
            return execute(dagdict, hash_, inputs)

    def __hash__(self):
        if not self.hash:
            parent_hashes = "#".join([ str(hash(p)) for p in self.parents ])
            self_hash = str(self.self_hash())
            self.hash = hash(type(self).__name__ + parent_hashes + self_hash)
        return self.hash

    def self_hash(self):
        return hash("")

    def get_inputs(self):
        ret = []
        for p in self.parents:
            ret += p.get_inputs()
        return ret

    def map(self, map_func):
        return Map(self, map_func)

    def flatten(self):
        return Flatten(self)

    def filter(self, predicate):
        return Filter(self, predicate)

    def flat_map(self, func):
        return FlatMap(self, func)

    def reduce_by_key(self, func):
        return ReduceByKey(self, func)

    def reduce(self, func):
        return Reduce(self, func).collect()[0]

    def join(self, other):
        return Join(self, other)

    def cartesian(self, other):
        return Cartesian(self, other)

    def collect(self):
        return self.executeDAG()

    def count(self):
        return self.flatten().map(lambda t: 1).reduce(lambda t1, t2: t1 + t2)

    def accept(self, visitor):

        def convert(name):
            s1 = first_cap_re.sub(r'\1_\2', name)
            return all_cap_re.sub(r'\1_\2', s1).lower()

        method_name = 'visit_{}'.format(convert(self.__class__.__name__))
        visit = getattr(visitor, method_name)
        return visit(self)


first_cap_re = re.compile('(.)([A-Z][a-z]+)')
all_cap_re = re.compile('([a-z0-9])([A-Z])')


class PipeRDD(RDD):
    def __init__(self, parent, func):
        super(PipeRDD, self).__init__(parent)
        self.func = func

    def self_hash(self):
        f = io.StringIO()
        dis.dis(self.func, file=f)
        return hash(f.getvalue())


class ShuffleRDD(RDD):
    pass


class Map(PipeRDD):
    def writeDAG(self, daglist, index):
        cur_index = super(Map, self).writeDAG(daglist, index)
        dic = self.dic
        dic[OP] = MAP
        dic[FUNC], self.output_type = get_llvm_ir_and_output_type(self.func, self.parents[0].output_type)
        # TODO(ingo): what output types are valid here?
        dic[OUTPUT_TYPE] = make_tuple(flatten(self.output_type))
        return cur_index


class Filter(PipeRDD):
    def writeDAG(self, daglist, index):
        cur_index = super(Filter, self).writeDAG(daglist, index)
        dic = self.dic
        dic[OP] = FILTER
        dic[FUNC], return_type = get_llvm_ir_and_output_type(self.func, self.parents[0].output_type)
        if str(return_type) != "bool":
            raise BaseException(
                    "Function given to reduce_by_key has the wrong return type:\n"
                    "  expected: {0}\n"
                    "  found:    {1}".format("bool", return_type))
        self.output_type = self.parents[0].output_type
        dic[OUTPUT_TYPE] = make_tuple(flatten(self.output_type))
        return cur_index


class FlatMap(PipeRDD):
    def writeDAG(self, daglist, index):
        cur_index = super(FlatMap, self).writeDAG(daglist, index)
        dic = self.dic
        dic[OP] = FLAT_MAP
        dic[FUNC], self.output_type = get_llvm_ir_output_type_generator(self.func)
        dic[OUTPUT_TYPE] = make_tuple(flatten(self.output_type))
        return cur_index


class Flatten(RDD):
    def writeDAG(self, daglist, index):
        cur_index = super(Flatten, self).writeDAG(daglist, index, True)
        self.output_type = make_tuple(flatten(self.parents[0].output_type))
        return cur_index


class Join(ShuffleRDD):
    """
    the first element in a tuple is the key
    """

    def __init__(self, left, right):
        super(Join, self).__init__(parent=left)
        self.parents.append(right)

    def compute_output_type(self):
        par_output_type1 = self.parents[0].output_type
        par_output_type2 = self.parents[1].output_type

        if isinstance(par_output_type1, BaseTuple):
            key_type = par_output_type1[0]
            output1 = [t for t in par_output_type1.types[1:]]
        else:
            key_type = par_output_type1
            output1 = None

        if isinstance(par_output_type2, BaseTuple):
            output2 = [t for t in par_output_type2.types[1:]]
        else:
            output2 = None

        if output1 is None:
            if output2 is None:
                return key_type
            else:
                output2.insert(0, key_type)
                return Tuple(output2)
        else:
            if output2 is None:
                output1.insert(0, key_type)
                return Tuple(output1)
            else:
                return Tuple([key_type, Tuple(flatten(output1) + flatten(output2))])

    def writeDAG(self, daglist, index):
        cur_index = super(Join, self).writeDAG(daglist, index)
        dic = self.dic
        dic[OP] = 'join'

        self.output_type = replace_unituple(self.compute_output_type())
        dic[OUTPUT_TYPE] = make_tuple(flatten(self.output_type))
        return cur_index


class Cartesian(ShuffleRDD):
    def __init__(self, left, right):
        super(Cartesian, self).__init__(parent=left)
        self.parents.append(right)

    def compute_output_type(self):
        p1 = make_tuple(self.parents[0].output_type) if len(self.parents[0].output_type) == 1 else self.parents[
            0].output_type
        p2 = make_tuple(self.parents[1].output_type) if len(self.parents[1].output_type) == 1 else self.parents[
            1].output_type
        return [p1, p2]

    def writeDAG(self, daglist, index):
        cur_index = super(Cartesian, self).writeDAG(daglist, index)
        dic = self.dic
        dic[OP] = 'cartesian'

        self.output_type = replace_unituple(self.compute_output_type())
        dic[OUTPUT_TYPE] = make_tuple(flatten(self.output_type))
        return cur_index


class Reduce(ShuffleRDD):
    """
    binary function must be commutative and associative
    the return value type should be the same as its arguments
    the input cannot be empty
    """

    def __init__(self, parent, func):
        super(Reduce, self).__init__(parent)
        self.func = func

    def self_hash(self):
        f = io.StringIO()
        dis.dis(self.func, file=f)
        return hash(f.getvalue())

    def computeInputType(self):
        # repeat the input type two times
        return [self.parents[0].output_type, self.parents[0].output_type]

    def writeDAG(self, daglist, index):
        cur_index = super(Reduce, self).writeDAG(daglist, index)
        dic = self.dic
        dic[OP] = REDUCE
        input_type = self.computeInputType()
        dic[FUNC], self.output_type = get_llvm_ir_and_output_type(self.func, input_type)
        if str(input_type[0]) != str(self.output_type):
            raise BaseException(
                    "Function given to reduce_by_key has the wrong return type:\n"
                    "  expected: {0}\n"
                    "  found:    {1}".format(input_type[0], self.output_type))
        dic[OUTPUT_TYPE] = make_tuple(flatten(self.output_type))
        return cur_index


class ReduceByKey(ShuffleRDD):
    """
    binary function must be commutative and associative
    the return value type should be the same as its arguments minus the key
    the input cannot be empty
    """

    def __init__(self, parent, func):
        super(ReduceByKey, self).__init__(parent)
        self.func = func

    def self_hash(self):
        f = io.StringIO()
        dis.dis(self.func, file=f)
        return hash(f.getvalue())

    def _compute_input_type(self):
        # repeat the input type two times minus the key
        par_type = self.parents[0].output_type

        def remove_key(type_):
            try:
                t = type_[0]
                if isinstance(t, BaseTuple):
                    r = remove_key(type_[0])
                    type_ = make_tuple([r] + list(type_[1:]))
                else:
                    type_ = make_tuple(type_[1:])
            except TypeError:
                pass
            return type_

        par_type = remove_key(par_type)
        if len(par_type) == 1:
            return [par_type[0], par_type[0]]
        return [par_type, par_type]

    def writeDAG(self, daglist, index):
        cur_index = super(ReduceByKey, self).writeDAG(daglist, index)
        dic = self.dic
        dic[OP] = REDUCEBYKEY
        input_type = self._compute_input_type()
        dic[FUNC], output_type = get_llvm_ir_and_output_type(self.func, input_type)
        if str(input_type[0]) != str(output_type):
            raise BaseException(
                    "Function given to reduce_by_key has the wrong return type:\n"
                    "  expected: {0}\n"
                    "  found:    {1}".format(input_type[0], output_type))
        self.output_type = self.parents[0].output_type
        dic[OUTPUT_TYPE] = make_tuple(flatten(self.output_type))
        return cur_index


class CSVSource(RDD):
    def __init__(self, path, delimiter=",", dtype=None, add_index=False):
        super(CSVSource, self).__init__()
        self.path = path
        self.dtype = dtype
        self.add_index = add_index
        self.delimiter = delimiter

    def self_hash(self):
        hash_objects = [self.path, str(self.dtype), str(self.add_index), delimiter,
                        self.compute_output()]
        return hash("#".join(hash_objects))

    def compute_output(self):
        df = np.genfromtxt(self.path, dtype=self.dtype, delimiter=self.delimiter, max_rows=1)

        try:
            self.output_type = replace_unituple(typeof(tuple(df[0])))
        except TypeError:  # scalar type
            self.output_type = typeof(df[0])

    def writeDAG(self, daglist, index):
        cur_index = super(CSVSource, self).writeDAG(daglist, index)
        self.output_type = self.compute_output()
        dic = self.dic
        dic[OP] = 'csv_source'
        dic[OUTPUT_TYPE] = make_tuple(flatten(self.output_type))
        dic[DATA_PATH] = self.path
        dic[ADD_INDEX] = self.add_index
        return cur_index


class CollectionSource(RDD):
    """
    accepts any python collections, numpy arrays or pandas dataframes

    passed python collection will be copied into a numpy array
    a reference to the array is stored in this instance
    to prevent freeing the input memory before the end of computation
    """

    def __init__(self, values, add_index=False):
        super(CollectionSource, self).__init__()

        if isinstance(values, DataFrame):
            self.array = values.values.ravel(order='C').reshape(values.shape)
        elif isinstance(values, ndarray):
            self.array = values
        else:
            self.output_type = replace_unituple(typeof(values[0]))
            self.array = np.array(values, dtype=numba_type_to_dtype(self.output_type))
        assert self.array.size > 0, "Empty collection not allowed"
        try:
            self.output_type = replace_unituple(typeof(tuple(self.array[0])))
        except TypeError:  # scalar type
            self.output_type = typeof(self.array[0])
        if add_index:
            self.output_type = replace_unituple(Tuple(flatten([numba.typeof(1), self.output_type])))
        self.data_ptr = self.array.__array_interface__['data'][0]
        self.size = self.array.shape[0]
        self.add_index = add_index

    def self_hash(self):
        hash_objects = [str(self.output_type), str(self.add_index)]
        return hash("#".join(hash_objects))

    def writeDAG(self, daglist, index):
        cur_index = super(CollectionSource, self).writeDAG(daglist, index)
        dic = self.dic
        dic[OP] = 'collection_source'
        dic[OUTPUT_TYPE] = make_tuple(flatten(self.output_type))
        dic[ADD_INDEX] = self.add_index

        return cur_index

    def get_inputs(self):
        return [(self.data_ptr, self.size)]


class RangeSource(RDD):
    def __init__(self, from_, to, step=1):
        super(RangeSource, self).__init__()
        self.from_ = from_
        self.to = to
        self.step = step
        self.output_type = typeof(step + from_)

    def self_hash(self):
        hash_objects = [str(o) for o in [self.from_, self.to, self.step, self.output_type]]
        return hash("#".join(hash_objects))

    def writeDAG(self, daglist, index):
        cur_index = super(RangeSource, self).writeDAG(daglist, index)
        dic = self.dic
        dic[OP] = 'range_source'
        dic[FROM] = self.from_
        dic[TO] = self.to
        dic[STEP] = self.step
        dic[OUTPUT_TYPE] = self.output_type
        return cur_index


class GeneratorSource(RDD):
    def __init__(self, func):
        super(GeneratorSource, self).__init__()
        self.func = func

    def self_hash(self):
        f = io.StringIO()
        dis.dis(self.func, file=f)
        return hash(f.getvalue())

    def writeDAG(self, daglist, index):
        cur_index = super(GeneratorSource, self).writeDAG(daglist, index)
        dic = self.dic
        dic[OP] = 'generator_source'
        dic[FUNC], self.output_type = get_llvm_ir_output_type_generator(self.func)
        dic[OUTPUT_TYPE] = self.output_type
        return cur_index
