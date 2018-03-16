import time


def mean(input_):
    return sum(input_) / len(input_)


def measure_time(func, max_rep=3, show_runs=False):
    res = []
    for i in range(0, max_rep):
        time_1 = time.perf_counter()
        func()
        time_2 = time.perf_counter()
        res.append(time_2 - time_1)
        if show_runs:
            print("run " + str(i) + " " + str(time_2 - time_1))
    return mean(res) * 1000


class Timer:
    def __init__(self):
        self._start = 0
        self._end = 0

    def start(self):
        self._start = time.perf_counter() * 1000

    def end(self):
        self._end = time.perf_counter() * 1000

    def diff(self):
        return str(self._end - self._start)
