from itertools import chain

from .operations import *
from timeit import default_timer as timer

DEBUG = False


def print_debug(msg):
    if DEBUG:
        print(msg)


def profile_decorator(func):
    def wrapper(self, values, *args, **kwargs):
        print_debug("Time %s function" % func.__name__)
        start = timer()
        res = func(self, values, *args, **kwargs)
        print_debug("function %s took: %f" % (func.__name__, (timer() - start)))
        return res

    return wrapper


def rdd_decorator(func):
    l_func = profile_decorator(func)

    def wrapper(self, *args, **kwargs):
        return RDD(self, lambda values: l_func(self, values, *args, **kwargs))

    return wrapper


class RDD:
    def __init__(self, parent, action=None, result=None):
        self.__parent = parent
        self.__action = action
        self.__result = result
        self.__cache = False

    def _compute(self):
        if self.__result:
            return self.__result
        else:
            assert self.__parent, "Parent RDD or the result must be set!"
            if self.__cache:
                if self.__action:
                    self.__result = self.__action(self.__parent._compute())
                else:
                    self.__result = self.__parent._compute()
                return self.__result
            else:
                p_result = self.__parent._compute()
                if self.__action:
                    return self.__action(p_result)
                else:
                    return p_result

    def cache(self):
        self.__cache = True
        return self

    @rdd_decorator
    def map(self, values, *args):
        return map_(args[0], values)

    @rdd_decorator
    def filter(self, values, *args):
        return filter_(args[0], values)

    @rdd_decorator
    def flat_map(self, values, *args):
        return list(chain.from_iterable(map(args[0], values)))

    @rdd_decorator
    def group_by_key(self, values):
        return group_by_key(values)

    @rdd_decorator
    def reduce_by_key(self, values, *args):
        return reduce_by_key(args[0], values)

    @rdd_decorator
    def sort(self, values, *args, **kwargs):
        return sorted(values, *args, **kwargs)

    @rdd_decorator
    def reduce(self, values, *args):
        return reduce(args[0], values)

    @rdd_decorator
    def join(self, values, *args):
        other = args[0]._compute()
        return join(values, other)

    def collect(self):

        return list(self._compute())
