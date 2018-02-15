from jitq.rdd import CollectionSource, RangeSource, GeneratorSource


class JitqContext:
    def __init__(self):
        self.serialization_cache = {}
        self.executor_cache = {}

    def read_csv(self, path, dtype=None):
        raise NotImplementedError

    def collection(self, values, add_index=False):
        return CollectionSource(self, values, add_index)

    def range_(self, from_, to, step=1):
        return RangeSource(self, from_, to, step)

    def generator(self, func):
        return GeneratorSource(self, func)
