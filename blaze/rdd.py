import json
import re

from blaze.c_executor import execute

from blaze.utils import *
from blaze.libs.numba.llvm_ir import cfunc
from blaze.constants import *

import numba
import numba.types as types
from numba.types.containers import *


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


def replace_unituple(type_):
    out = type_
    if isinstance(type_, Tuple):
        type_type = type(type_)
        child_types = []
        for child_type in type_.types:
            child_types.append(replace_unituple(child_type))
        out = type_type(child_types)
    elif isinstance(type_, UniTuple):
        type_type = Tuple([])
        type_type.types = (replace_unituple(type_.dtype),) * type_.count
        type_type.count = type_.count
        type_type.name = "(%s)" % ', '.join(str(i) for i in type_type.types)
        out = type_type
    elif isinstance(type_, (list, tuple)):
        out = tuple(map(lambda t: replace_unituple(t), type_))
    return out


def get_llvm_ir_and_output_type(func, input_type=None):
    print("old input type: " + str(input_type))
    input_type = replace_unituple(input_type)
    print("new input type: " + str(input_type))

    # get the output type with njit
    dec_func = numba.njit((input_type,))(func)
    # llvm = dec_func.inspect_llvm()
    # for k, v in llvm.items():
    #     # print(str(v))
    #     pass
    # # # file.write(v)
    # #     # file.write('\n')
    output_type = dec_func.nopython_signatures[0].return_type

    # print("old output type" + str(output_type))
    output_type = replace_unituple(output_type)
    # print("new output type" + str(output_type))

    cfunc_code = cfunc(output_type(input_type))(func)
    code = cfunc_code.inspect_llvm()
    # Extract just the code of the function
    m = re.search('define [^\n\r]* @"cfunc.*\n\n', code, re.DOTALL)
    code_string = m.group(0)

    return code_string, output_type


def get_llvm_ir_output_type_generator(func):
    dec_func = numba.njit(())(func)
    output_type = dec_func.nopython_signatures[0].return_type
    llvm = dec_func.inspect_llvm()[()]
    output_type = output_type.yield_type
    return llvm, output_type


class RDD(object):
    def __init__(self, parent=None):
        self.dic = None
        self._cache = False
        if parent:
            self.parents = [parent]
        else:
            self.parents = []
        self.output_type = None

    def cache(self):
        self.cache = True
        return self

    def writeDAG(self, daglist, index):
        if self.dic:
            return self.dic[ID]
        cur_index = index
        preds_index = []
        for par in self.parents:
            pred_index = par.writeDAG(daglist, cur_index)
            cur_index = max(pred_index + 1, cur_index)
            preds_index.append(pred_index)

        dic = dict()
        dic[ID] = cur_index
        dic[PREDS] = preds_index
        self.dic = dic
        daglist.append(dic)
        return cur_index

    def startDAG(self, action):
        dagdict = dict()
        dagdict[ACTION] = action
        dagdict[DAG] = []

        cleanRDDs(self)
        self.writeDAG(dagdict[DAG], 0)
        # write to file
        # fp = open('dag.json', 'w')
        # json.dump(dagdict, fp=fp, cls=RDDEncoder)
        res = execute(dagdict)
        return res

    def map(self, map_func):
        return Map(self, map_func)

    def filter(self, predicate):
        return Filter(self, predicate)

    def flat_map(self, func):
        return FlatMap(self, func)

    def reduce(self, func):
        return Reduce(self, func)

    def join(self, other):
        return Join(self, other)

    def collect(self):
        res = self.startDAG("collect")
        return res

    def count(self):
        return self.startDAG("count")

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


class ShuffleRDD(RDD):
    pass


class Map(PipeRDD):
    def writeDAG(self, daglist, index):
        if self.dic:
            return self.dic[ID]
        cur_index = super(Map, self).writeDAG(daglist, index)
        dic = self.dic
        dic[OP] = MAP
        dic[FUNC], self.output_type = get_llvm_ir_and_output_type(self.func, self.parents[0].output_type)
        dic[OUTPUT_TYPE] = self.output_type
        return cur_index


class Filter(PipeRDD):
    def writeDAG(self, daglist, index):
        if self.dic:
            return self.dic[ID]
        cur_index = super(Filter, self).writeDAG(daglist, index)
        dic = self.dic
        dic[OP] = FILTER
        dic[FUNC], _ = get_llvm_ir_and_output_type(self.func, self.parents[0].output_type)
        self.output_type = self.parents[0].output_type
        dic[OUTPUT_TYPE] = self.output_type
        return cur_index


class FlatMap(PipeRDD):
    def writeDAG(self, daglist, index):
        if self.dic:
            return self.dic[ID]
        cur_index = super(FlatMap, self).writeDAG(daglist, index)
        dic = self.dic
        dic[OP] = FLAT_MAP
        dic[FUNC], self.output_type = get_llvm_ir_output_type_generator(self.func)
        dic[OUTPUT_TYPE] = self.output_type
        return cur_index


class Join(ShuffleRDD):
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
                output1 += output2
                return Tuple([key_type, Tuple(output1)])

    def writeDAG(self, daglist, index):
        if self.dic:
            return self.dic[ID]
        cur_index = super(Join, self).writeDAG(daglist, index)
        dic = self.dic
        dic[OP] = 'join'
        # dic[FUNC] = 'function code join'
        # (K, V), (K, W) => (K, (V, W))

        self.output_type = self.compute_output_type()
        dic[OUTPUT_TYPE] = self.output_type
        return cur_index


class Reduce(ShuffleRDD):
    def __init__(self, parent, func):
        super(Reduce, self).__init__(parent)
        self.func = func

    def writeDAG(self, daglist, index):
        if self.dic:
            return self.dic[ID]
        cur_index = super(Reduce, self).writeDAG(daglist, index)
        dic = self.dic
        dic[OP] = 'reduce'
        dic[FUNC] = 'function code reduce'

        return cur_index


class TextSource(RDD):
    def __init__(self, path):
        super(TextSource, self).__init__()
        self.path = path


class CollectionSource(RDD):
    """
    passed collection is copied into a numpy array
    a reference to the array is stored in this instance
    to prevent freeing input memory before the end of computation
    """

    def __init__(self, values):
        assert values, "Empty collection not allowed"
        super(CollectionSource, self).__init__()
        self.output_type = replace_unituple(numba.typeof(values[0]))
        self.array = np.array(values, dtype=numba_type_to_dtype(self.output_type))
        self.data_ptr = self.array.__array_interface__['data'][0]
        self.size = self.array.size

    def writeDAG(self, daglist, index):
        if self.dic:
            return self.dic[ID]
        cur_index = super(CollectionSource, self).writeDAG(daglist, index)
        dic = self.dic
        dic[OP] = 'collection_source'
        dic[OUTPUT_TYPE] = self.output_type
        dic[DATA_PTR] = self.data_ptr
        dic[DATA_SIZE] = self.size

        return cur_index


class NumpyArraySource(RDD):
    def __init__(self, array):
        super(NumpyArraySource, self).__init__()
        try:
            self.output_type = replace_unituple(typeof(tuple(array[0])))
        except TypeError:  # scalar type
            self.output_type = typeof(array[0])
        self.size = array.size
        self.data_ptr = array.__array_interface__['data'][0]
        # TODO we probably have to keep a reference to the np array
        # until the end of the execution to prevent gc

    def writeDAG(self, daglist, index):
        if self.dic:
            return self.dic[ID]
        cur_index = super(NumpyArraySource, self).writeDAG(daglist, index)
        dic = self.dic
        dic[OP] = 'collection_source'
        dic[DATA_PTR] = self.data_ptr
        dic[DATA_SIZE] = self.size
        dic[OUTPUT_TYPE] = self.output_type
        return cur_index


class RangeSource(RDD):
    def __init__(self, from_, to, step=1):
        super(RangeSource, self).__init__()
        self.from_ = from_
        self.to = to
        self.step = step
        self.output_type = typeof(step + from_)

    def writeDAG(self, daglist, index):
        if self.dic:
            return self.dic[ID]
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

    def writeDAG(self, daglist, index):
        if self.dic:
            return self.dic[ID]
        cur_index = super(GeneratorSource, self).writeDAG(daglist, index)
        dic = self.dic
        dic[OP] = 'generator_source'
        dic[FUNC], self.output_type = get_llvm_ir_output_type_generator(self.func)
        dic[OUTPUT_TYPE] = self.output_type
        return cur_index
