from scipy.spatial import distance
from sklearn.datasets.samples_generator import make_blobs
from sklearn.neighbors import KNeighborsClassifier

from jitq.benchmarks.timer import measure_time

N_FEATURES = 10
N_SAMPLES = 1 << 10
N_CLUSTERS = 5

N_NEIGHBOURS = 5
RANDOM_STATE = 100
RUNS = 3

X, Y = make_blobs(N_SAMPLES, N_FEATURES, N_CLUSTERS, random_state=RANDOM_STATE)
print("generated benchmark data")


def chebyshev_dist(tuple_1, tuple_2):
    max_ = 0
    n_cols = len(tuple_1)
    for i in range(n_cols):
        dist = abs(tuple_1[i] - tuple_2[i])
        max_ = max(dist, max_)
    return max_


def euclidian_dist(tuple_1, tuple_2):
    dist = 0
    for _x1, _x2 in zip(tuple_1, tuple_2):
        dist += (_x1 - _x2)**2
    return dist


def builtin_euclidian():
    classifier = KNeighborsClassifier(n_neighbors=N_NEIGHBOURS)
    classifier.fit(X, Y)

    def run():
        classifier.fit(X, Y)
        return classifier.predict(X)

    return run


def builtin_chebyshev():
    classifier = KNeighborsClassifier(
        n_neighbors=N_NEIGHBOURS, metric="chebyshev")
    classifier.fit(X, Y)

    def run():
        classifier.fit(X, Y)
        return classifier.predict(X)

    return run


def custom_metric_python():
    classifier = KNeighborsClassifier(
        n_neighbors=N_NEIGHBOURS, metric=euclidian_dist)
    classifier.fit(X, Y)

    def run():
        classifier.fit(X, Y)
        return classifier.predict(X)

    return run


def custom_metric_numpy():
    classifier = KNeighborsClassifier(
        n_neighbors=N_NEIGHBOURS, metric=distance.sqeuclidean)
    classifier.fit(X, Y)

    def run():
        classifier.fit(X, Y)
        return classifier.predict(X)

    return run


def custom_metric_chebyshev():
    classifier = KNeighborsClassifier(
        n_neighbors=N_NEIGHBOURS, metric=chebyshev_dist)
    classifier.fit(X, Y)

    def run():
        classifier.fit(X, Y)
        return classifier.predict(X)

    return run


def custom_metric_numpy_chebyshev():
    classifier = KNeighborsClassifier(
        n_neighbors=N_NEIGHBOURS, metric=distance.chebyshev)

    def run():
        classifier.fit(X, Y)
        return classifier.predict(X)

    return run


def run_benchmarks():
    elapsed_time = measure_time(builtin_euclidian(), RUNS)
    print("knn built-in euclidian " + str(elapsed_time))

    elapsed_time = measure_time(builtin_chebyshev(), RUNS)
    print("knn built-in chebyshev " + str(elapsed_time))

    elapsed_time = measure_time(custom_metric_numpy(), RUNS)
    print("knn custom metric numpy " + str(elapsed_time))

    elapsed_time = measure_time(custom_metric_python(), RUNS)
    print("knn custom metric python " + str(elapsed_time))

    elapsed_time = measure_time(custom_metric_numpy_chebyshev(), RUNS)
    print("knn custom metric numpy chebyshev " + str(elapsed_time))

    elapsed_time = measure_time(custom_metric_chebyshev(), RUNS)
    print("knn custom metric python chebyshev " + str(elapsed_time))


if __name__ == '__main__':
    run_benchmarks()
