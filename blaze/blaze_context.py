from blaze.rdd import TextSource, CollectionSource, RangeSource, GeneratorSource


def text_file(path):
    return TextSource(path)


def collection(values):
    return CollectionSource(values)


def range_(from_, to, step=1):
    return RangeSource(from_, to, step)


def generator(func):
    return GeneratorSource(func)
