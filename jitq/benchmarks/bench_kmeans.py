from sklearn.datasets.samples_generator import make_blobs
from sklearn.cluster import KMeans as sk_KMeans

import matplotlib.pyplot as plt

import numpy as np

from jitq.benchmarks.timer import measure_time
from ml.kmeans import KMeans

N_FEATURES = 30
N_SAMPLES = 1 << 16
N_CLUSTERS = 5

N_INIT = 1
MAX_ITER = 3

RANDOM_STATE = 111
INIT = 'random'

X, Y = make_blobs(N_SAMPLES, N_FEATURES, N_CLUSTERS, random_state=RANDOM_STATE)
print("generated data")

plt.scatter(X[:, 0], X[:, 1], c=Y.astype(np.float))


def bench_sklearn_kmeans():
    def run():
        classifier = sk_KMeans(
            N_CLUSTERS,
            n_init=N_INIT,
            max_iter=MAX_ITER,
            random_state=RANDOM_STATE,
            init=INIT,
            algorithm='full',
            precompute_distances=False)
        classifier.fit(X)

    return run


def chebyshev_dist(point1, point2):
    max_ = 0
    n_cols = len(point1)
    for i in range(n_cols):
        dist = abs(point1[i] - point2[i])
        max_ = max(dist, max_)
    return max_


def bench_kmeans():
    def run():
        classifier = KMeans(
            N_CLUSTERS,
            n_init=N_INIT,
            max_iter=MAX_ITER,
            init=INIT,
            random_state=RANDOM_STATE)
        classifier.fit(X)
        return classifier

    return run


print("time kmeans  " + str(measure_time(bench_kmeans(), 1) / MAX_ITER))

plt.show()
