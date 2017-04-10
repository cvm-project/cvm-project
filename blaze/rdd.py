from itertools import chain
from .operations import *


def rdd_decorator(func):

    def wrapper(self, *args, **kwargs):
        return RDD(self, lambda values: func(self, values, *args, **kwargs))

    return wrapper


class RDD:
    def __init__(self, parent, action, result=None):
        self.__parent = parent
        self.__action = action
        self.__result = result
        self.__cache = False

    def __compute(self):
        if self.__result:
            return self.__result
        else:
            assert self.__parent, "Parent RDD or the result must be set!"
            if self.__cache:
                self.__result = self.__action(self.__parent.collect())
                return self.__result
            else:
                p_result = self.__parent.collect()
                return self.__action(p_result)

    def cache(self):
        self.__cache = True
        return self

    @rdd_decorator
    def map(self, values, *args):
        return list(map(args[0], values))

    @rdd_decorator
    def filter(self, values, *args):
        return filter(args[0], values)

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
        other = args[0].collect()
        return join(values, other)

    def collect(self):
        return self.__compute()
