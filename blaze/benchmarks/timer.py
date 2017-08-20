import time


def mean(l):
    return sum(l) / len(l)


def timer(func, max_rep=3):
    res = []
    for r in range(0, max_rep):
        # print("run timer")
        t1 = time.perf_counter()
        resu = func()
        t2 = time.perf_counter()
        res.append(t2 - t1)
        # print(resu)
    return mean(res)


class Timer:
    def __init__(self):
        self._start = 0
        self._end = 0

    def start(self):
        self._start = time.perf_counter()

    def end(self):
        self._end = time.perf_counter()

    def diff(self):
        return self._end - self._start
