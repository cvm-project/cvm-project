from blaze.rdd import TextSource, CollectionSource, RangeSource, GeneratorSource, NumpyArraySource


class BlazeContext:
    @staticmethod
    def text_file(path):
        return TextSource(path)

    @staticmethod
    def collection(values, add_index=False):
        return CollectionSource(values, add_index)

    @staticmethod
    def range_(from_, to, step=1):
        return RangeSource(from_, to, step)

    @staticmethod
    def generator(func):
        return GeneratorSource(func)

    @staticmethod
    def numpy_array(array, add_index=False):
        return NumpyArraySource(array, add_index)
