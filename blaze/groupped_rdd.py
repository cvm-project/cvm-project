from blaze.rdd import RDD, rdd_decorator
from blaze.operations import *


class GroupedRDD(RDD):
    @rdd_decorator
    def join(self, values, *args):
        other = args[0]
        return other.grouped_join(values, self)

    def grouped_join(self, values, *args):
        # both relations are grouped
        return join(values, args[0]._compute())
