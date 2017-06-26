import re

from blaze.controller import compute_sink_rdd
from .transforms import *

DEBUG = False


def print_debug(msg):
    if DEBUG:
        print(msg)


class RDD(object):
    def __init__(self, parent=None, result=None):
        if parent:
            self.parents = [parent]
            parent.successor = self
        else:
            self.parents = []
        self._result = result
        self._cache = False
        self.successor = None

    def _compute(self):
        if self._result:
            return self._result
        else:
            return compute_sink_rdd(self)

    def cache(self):
        self._cache = True
        return self

    def map(self, map_func):
        return Map(self, map_func)

    def filter(self, predicate):
        return Filter(self, predicate)

    def flat_map(self, func):
        return FlatMap(self, func)

    # @rdd_decorator
    # def group_by_key(self, values):
    #     return group_by_key(values)
    #
    # @rdd_decorator
    # def reduce_by_key(self, values, *args):
    #     return reduce_by_key(args[0], values)
    #
    # @rdd_decorator
    # def sort(self, values, *args, **kwargs):
    #     return sorted(values, *args, **kwargs)
    #
    def reduce(self, func):
        return Reduce(self, func)

    def join(self, other):
        return Join(self, other)

    def collect(self):
        return self._compute()

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
    pass


class Filter(PipeRDD):
    pass


class FlatMap(PipeRDD):
    pass


class Join(ShuffleRDD):
    def __init__(self, left, right):
        super(Join, self).__init__(parent=left)
        self.parents.append(right)
        right.successor = self
        self.hash_right = True


class TextSource(RDD):
    def __init__(self, path):
        super(TextSource, self).__init__()
        self.path = path


class CollectionSource(RDD):
    def __init__(self, values):
        super(CollectionSource, self).__init__()
        self.values = values


class Reduce(ShuffleRDD):
    pass
