from scipy.spatial import distance
from sklearn.cluster import DBSCAN
from sklearn.datasets.samples_generator import make_blobs
from sklearn.neighbors import KNeighborsClassifier

from blaze.blaze_context import BlazeContext
from blaze.benchmarks.timer import timer
from sklearn.cluster import KMeans as sk_KMeans
import numpy as np
import matplotlib.pyplot as plt
from ml.kmeans import KMeans

n_features = 5
n_samples = 1 << 12
n_clusters = 5

n_neighbours = 5
random_state = 100

x, y = make_blobs(n_samples, n_features, n_clusters, random_state=random_state)
print("generated data")


def chebyshev_dist(t1, t2):
    max_ = 0
    n_cols = len(t1)
    for i in range(n_cols):
        dist = abs(t1[i] - t2[i])
        max_ = max(dist, max_)
    return max_


def euclidian_dist(t1, t2):
    dist = 0
    for i in range(len(t1)):
        _x1 = t1[i]
        _x2 = t2[i]
        dist += (_x1 - _x2) ** 2
    return dist


def bench_sklearn_knn():
    classifier = KNeighborsClassifier(n_neighbors=n_neighbours)
    classifier.fit(x, y)

    def run():
        return classifier.predict(x)
        pass

    return run


def bench_sklearn_knn_custom_metric():
    classifier = KNeighborsClassifier(n_neighbors=n_neighbours, metric=euclidian_dist)
    classifier.fit(x, y)

    def run():
        return classifier.predict(x)
        pass

    return run


def bench_sklearn_knn_custom_metric_numpy():
    classifier = KNeighborsClassifier(n_neighbors=n_neighbours, metric=distance.sqeuclidean)
    classifier.fit(x, y)

    def run():
        return classifier.predict(x)
        pass

    return run


def bench_sklearn_knn_custom_metric_chebyshev():
    classifier = KNeighborsClassifier(n_neighbors=n_neighbours, metric=chebyshev_dist)
    classifier.fit(x, y)

    def run():
        return classifier.predict(x)
        pass

    return run


def bench_sklearn_knn_custom_metric_numpy_chebyshev():
    classifier = KNeighborsClassifier(n_neighbors=n_neighbours, metric=distance.chebyshev)
    classifier.fit(x, y)

    def run():
        return classifier.predict(x)
        pass

    return run


runs = 3
#
t = timer(bench_sklearn_knn(), runs)
print("knn default " + str(t))
#
t = timer(bench_sklearn_knn_custom_metric_numpy(), runs)
print("knn custom metric numpy " + str(t))
#
t = timer(bench_sklearn_knn_custom_metric(), runs)
print("knn custom metric python " + str(t))

t = timer(bench_sklearn_knn_custom_metric_numpy_chebyshev(), runs)
print("knn custom metric numpy chebyshev " + str(t))

t = timer(bench_sklearn_knn_custom_metric_chebyshev(), runs)
print("knn custom metric python chebyshev " + str(t))
