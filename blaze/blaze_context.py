from blaze.rdd import RDD


class BlazeContext:
    def text_file(self, path):
        """
        read a text file line by line
        return an RDD with elements lines of the text
        """

        return RDD(TextRDD(path))

    def collection(self, values):
        return RDD(CollectionRDD(values))


class TextRDD:
    def __init__(self, path):
        self.__path = path

    def _compute(self):
        with open(self.__path, mode="r", encoding='utf-8') as file:
            data = file.readlines()
        return data

    def collect(self):
        return self._compute()


class CollectionRDD:
    def __init__(self, values):
        self.__values = values

    def _compute(self):
        return self.__values

    def collect(self):
        return self._compute()

