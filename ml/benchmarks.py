from sklearn.datasets.samples_generator import make_blobs
from blaze.blaze_context import BlazeContext
from blaze.benchmarks.timer import timer
from sklearn.cluster import KMeans as sk_KMeans
import numpy as np
import matplotlib.pyplot as plt
from ml.kmeans import KMeans

n_features = 2
n_samples = 200
n_clusters = 10

n_init = 1
max_iter = 1

random_state = 77

x, y = make_blobs(n_samples, n_features, n_clusters, random_state=random_state)


# plt.scatter(x[:,0], x[:, 1], c=y.astype(np.float))
# plt.show()
def bench_sklearn_kmeans():
    classifier = sk_KMeans(n_clusters, n_init=n_init, max_iter=max_iter, random_state=random_state)
    classifier.fit(x)
    print ("error: " + str(classifier.inertia_))

    def run():
        return classifier.fit(x)

    return run


def bench_kmeans():
    classifier = KMeans(n_clusters, n_init=n_init, max_iter=max_iter)

    def run():
        return classifier.fit(x)

    return run


t = timer(bench_kmeans(), 1)
print("time kmeans  " + str(t))
#
# t = timer(bench_sklearn_kmeans(), 1)
# print("time sklearn kmeans  " + str(t))
