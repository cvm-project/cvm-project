import os
import sys

sys.path.insert(0, os.environ['JITQPATH'])

from sklearn.datasets.samples_generator import make_blobs
from jitq.jitq_context import JitqContext
from jitq.benchmarks.timer import timer
from sklearn.cluster import KMeans as sk_KMeans
import numpy as np
import matplotlib.pyplot as plt
from ml.kmeans import KMeans

n_features = 30
n_samples = 1 << 20
n_clusters = 5

n_init = 1
max_iter = 3

random_state = 111
# init = 'k-means++'
init = 'random'

x, y = make_blobs(n_samples, n_features, n_clusters, random_state=random_state)
# print("true labels " + str(y))
print("generated data")


# plt.scatter(x[:, 0], x[:, 1], c=y.astype(np.float))


def bench_sklearn_kmeans():
    def run():
        classifier = sk_KMeans(n_clusters, n_init=n_init, max_iter=max_iter, random_state=random_state, init=init,
                               algorithm='full', precompute_distances=False)
        classifier.fit(x)

    return run


def chebyshev_dist(t1, t2):
    max_ = 0
    n_cols = len(t1)
    for i in range(n_cols):
        dist = abs(t1[i] - t2[i])
        max_ = max(dist, max_)
    return max_


def bench_kmeans():
    def run():
        classifier = KMeans(n_clusters, n_init=n_init, max_iter=max_iter, init=init, random_state=random_state)
        classifier.fit(x)
        return classifier

    return run


# t = timer(bench_sklearn_kmeans(), 1)
# print("time sklearn kmeans  " + str(t/max_iter))

#
t = timer(bench_kmeans(), 1)
print("time kmeans  " + str(t/max_iter))
#

plt.show()
