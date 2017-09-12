from sklearn.datasets.samples_generator import make_blobs
from blaze.blaze_context import BlazeContext
from blaze.benchmarks.timer import timer
from sklearn.cluster import KMeans as sk_KMeans
import numpy as np
import matplotlib.pyplot as plt
from ml.kmeans import KMeans

n_features = 2
n_samples = 1000
n_clusters = 10

n_init = 1
max_iter = 100

random_state = 42

x, y = make_blobs(n_samples, n_features, n_clusters, random_state=random_state)
# print("true labels " + str(y))


plt.scatter(x[:, 0], x[:, 1], c=y.astype(np.float))


def bench_sklearn_kmeans():
    classifier = sk_KMeans(n_clusters, n_init=n_init, max_iter=max_iter, random_state=random_state, init='random',
                           algorithm='full')
    classifier.fit(x)
    print("error: " + str(classifier.inertia_))
    print("centroids: " + str(classifier.cluster_centers_))

    plt.scatter(classifier.cluster_centers_[:, 0], classifier.cluster_centers_[:, 1], s=100)

    def run():
        # return classifier.fit(x)
        pass

    return run


def bench_kmeans():
    classifier = KMeans(n_clusters, n_init=n_init, max_iter=max_iter, init='random', random_state=random_state)
    classifier.fit(x)
    print("error soldr: " + str(classifier.inertia_))
    print("centroids soldr: " + str(classifier.cluster_centers_))
    centrs = classifier.cluster_centers_
    plt.scatter(classifier.cluster_centers_[:, 0], classifier.cluster_centers_[:, 1], s=200)

    def run():
        # return classifier.fit(x)
        pass

    return run


#
t = timer(bench_kmeans(), 1)
print("time kmeans  " + str(t))
#
t = timer(bench_sklearn_kmeans(), 1)
print("time sklearn kmeans  " + str(t))

plt.show()
