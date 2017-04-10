from blaze.rdd import RDD


class BlazeContext:
    def __init__(self):
        self.__path = None

    def text_file(self, path):
        """
        read a text file line by line
        return an RDD with elements lines of the text
        """
        self.__path = path
        return RDD(self, lambda i: i)

    def collect(self):
        with open(self.__path, mode="r", encoding='utf-8') as file:
            data = file.readlines()
        return data
