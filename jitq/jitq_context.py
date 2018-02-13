from jitq.rdd import CollectionSource, RangeSource, GeneratorSource, CSVSource


class JitqContext:
    @staticmethod
    def read_csv(path, dtype=None):
        raise NotImplemented

    @staticmethod
    def collection(values, add_index=False):
        return CollectionSource(values, add_index)

    @staticmethod
    def range_(from_, to, step=1):
        return RangeSource(from_, to, step)

    @staticmethod
    def generator(func):
        return GeneratorSource(func)
