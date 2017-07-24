class Transform:
    def __init__(self, successor=None, func=None):
        self._successor = successor
        self._schema = None
        self.func = func

    @property
    def successor(self):
        return self._successor

    @successor.setter
    def successor(self, successor):
        assert successor != self
        self._successor = successor


class PipeTransform(Transform):
    pass


class Map(PipeTransform):
    pass


class Filter(PipeTransform):
    pass


class FlatMap(PipeTransform):
    pass


class ShuffleTransform(Transform):
    pass


class Join(ShuffleTransform):
    def __init__(self, other, successor=None, func=None):
        super(Join).__init__(successor, func)
        self.__other = other
