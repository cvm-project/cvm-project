import json
from functools import reduce

import numpy as np
from cffi import FFI
from numba import types

from jitq import backend
from jitq.rdd_result import NumpyResult
from jitq.utils import RDDEncoder, Timer, \
    numba_type_to_dtype, get_type_size


# pylint: disable=inconsistent-return-statements
def wrap_result(res, type_):

    values = json.loads(res.string)
    assert len(values) == 1

    if values[0]['type'] == 'none':
        return None

    assert values[0]['type'] == 'tuple'
    result = values[0]['fields']

    if isinstance(type_, types.Array):
        assert len(result) == 1
        outer_shape = result[0]['outer_shape']
        shape = result[0]['shape']
        assert type_.ndim == len(outer_shape)
        for (size1, size2) in zip(outer_shape, shape):
            assert size1 == size2
        for offset in result[0]['offsets']:
            assert offset == 0
        dtype_size = get_type_size(type_.dtype)
        total_count = reduce(lambda t1, t2: t1 * t2, outer_shape)
        ffi = FFI()
        c_buffer = ffi.buffer(ffi.cast("void *", result[0]['data']),
                              total_count * dtype_size)
        np_arr = np.frombuffer(c_buffer,
                               dtype=numba_type_to_dtype(type_.dtype),
                               count=total_count)
        np_arr = np_arr.view(NumpyResult)
        np_arr.handle = res

        return np_arr

    if isinstance(type_, types.Tuple):
        return tuple((field['value'] for field in result))

    if isinstance(type_, (types.Integer, types.Float, types.Boolean,
                          types.misc.UnicodeType)):
        assert len(result) == 1
        return result[0]['value']

    assert False


class ResultHandle:
    def __init__(self, string):
        self.string = string

    def __del__(self):
        backend.FreeResult(self.string)


def lookup_or_generate_plan(context, dag_str, conf_str):
    cache_key = dag_str + conf_str
    plan_id = context.executor_cache.get(cache_key, None)
    if not plan_id:
        timer = Timer()
        timer.start()
        plan_id = backend.GenerateExecutable(conf_str, dag_str)
        timer.end()
        print("time: calling make " + str(timer.diff()))
        context.executor_cache[cache_key] = plan_id
    return plan_id


def execute(context, dag_dict, inputs, output_type):
    args_str = json.dumps(inputs)
    dag_str = json.dumps(dag_dict, cls=RDDEncoder)
    conf_str = json.dumps(context.conf)

    plan_id = lookup_or_generate_plan(context, dag_str, conf_str)

    timer = Timer()
    timer.start()
    res = ResultHandle(backend.ExecutePlan(plan_id, args_str))

    timer.end()
    print("execute " + str(timer.diff()))
    res = wrap_result(res, output_type)

    return res
