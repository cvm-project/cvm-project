
from blaze.rdd import TextSource, CollectionSource
import numpy as np


class BlazeContext:
    def text_file(self, path):
        """
        read a text file line by line
        return an RDD with elements lines of the text
        """

        return TextSource(path)

    def collection(self, values):
        return CollectionSource(values)

