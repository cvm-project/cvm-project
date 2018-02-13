from math import sqrt

import sys
from six import string_types
from sklearn.metrics import euclidean_distances
from sklearn.utils import check_random_state
from sklearn.utils.extmath import row_norms, squared_norm
from sklearn.utils.sparsefuncs import mean_variance_axis

import numba
import numpy as np
import scipy.sparse as sp

from jitq.ast_optimizer import unroll_loops
from jitq.benchmarks.timer import Timer
from jitq.jitq_context import JitqContext
from numba import njit


def _tolerance(X, tol):
    """Return a tolerance which is independent of the dataset"""
    if sp.issparse(X):
        variances = mean_variance_axis(X, axis=0)[1]
    else:
        variances = np.var(X, axis=0)
    return np.mean(variances) * tol


@njit(cache=True)
def _euclidian_dist(t1, t2):
    dist = 0
    for i in range(len(t1)):
        _x1 = t1[i]
        _x2 = t2[i]
        dist += (_x1 - _x2) ** 2
    return dist

_metric = _euclidian_dist

class KMeans():
    def __init__(self, n_clusters=8, n_init=10,
                 max_iter=300, tol=1e-4, random_state=None, init='kmeans++', metric="euclidian"):
        self.n_clusters = n_clusters
        self.n_init = n_init
        self.max_iter = max_iter
        self.tol = 1e-4
        self.random_state = random_state
        self.cluster_centers_ = []
        self.init = init
        global _metric
        if metric == "euclidian":
            _metric = _euclidian_dist
        else:
            if not hasattr(metric, "__call__"):
                raise AttributeError("metric must be \"euclidian\" or a callable")
            _metric = njit(metric, cache=True)

    def fit(self, X, y=None):
        n_cols = len(X[0])
        n_samples = len(X[1])
        bc = JitqContext()
        old_centroids = _init_centroids(X, self.n_clusters, init=self.init, random_state=self.random_state)
        centroids = bc.collection(old_centroids, add_index=True)
        old_centroids = centroids.collect()
        self.tol = _tolerance(X, self.tol)
        in_ = bc.collection(X, add_index=True)

        def reduce_1(t1, t2):
            point = t1[0][:]
            old_centre = t1[1][:]
            old_dist = t1[2]

            new_centre = t2[1][:]

            new_dist = t2[2]
            if new_dist < old_dist:
                return point, new_centre, new_dist
            else:
                return point, old_centre, old_dist

        @unroll_loops
        def reduce_2(t1, t2):
            sum_ = t1[1]
            mean_c = t1[0][:]
            new_point = t2[2][:]
            res = ()
            for z in range(n_cols):
                n = mean_c[z] + new_point[z]
                res += (n,)
            return res, sum_ + t2[1], new_point

        @unroll_loops
        def map_1(t):
            s = t[1]
            center = t[0][1:]
            res = ()
            for z in range(n_cols):
                n = center[z] / s
                res += (n,)
            return t[0][0], res

        @unroll_loops
        def map_0(t1, t2):
            # dist = 0
            # for j in range(n_cols):
            #     _x1 = t1[1:][j]
            #     _x2 = t2[1:][j]
            #     dist += (_x1 - _x2) ** 2
            # return t2, t1, dist
            return t2, t1, _metric(t1[1:], t2[1:])

        for i in range(self.max_iter):
            # iteration time: 7000
            cart = centroids.cartesian(in_)
            # put points first to reduce on them
            cart = cart.map(map_0)
            # for every point compute the closest centroid (E step)
            pnt_center = cart.reduce_by_key(reduce_1)
            # put centres first to reduce on them,
            # use the point index as the accumulator
            centr_pnt = pnt_center.map(lambda *t: (t[1], 1, t[0][1:]))
            # reassign each centroid to the mean (M step)
            centr_pnt = centr_pnt.reduce_by_key(reduce_2)
            # project the new centroids and compute the mean
            centroids = centr_pnt.map(map_1)
            # # compute the error
            new_centroids = centroids.collect()
            centroids = bc.collection(new_centroids)
            shift = squared_norm(new_centroids.view(np.float64) - old_centroids.view(np.float64))
            old_centroids = new_centroids
            if shift < self.tol:
                break

        print("total iterations solrd: " + str(i + 1))
        self.cluster_centers_ = old_centroids[list(old_centroids.dtype.names[1:])].view(np.float64).reshape(
            (self.n_clusters, n_cols))
        # compute labels and inertia
        cart = bc.collection(old_centroids).cartesian(in_)
        # put points first to reduce on them
        cart = cart.map(map_0)
        # for every point compute the closest centroid (E step)
        pnt_center_distance = cart.reduce_by_key(reduce_1).collect()

        last_col_index = pnt_center_distance.dtype.names[-1]
        self.inertia_ = np.sum(pnt_center_distance[last_col_index], dtype=np.float64)


def _k_init(X, n_clusters, x_squared_norms, random_state, n_local_trials=None):
    """Init n_clusters seeds according to k-means++

    Parameters
    -----------
    X: array or sparse matrix, shape (n_samples, n_features)
        The data to pick seeds for. To avoid memory copy, the input data
        should be double precision (dtype=np.float64).

    n_clusters: integer
        The number of seeds to choose

    x_squared_norms: array, shape (n_samples,)
        Squared Euclidean norm of each data point.

    random_state: numpy.RandomState
        The generator used to initialize the centers.

    n_local_trials: integer, optional
        The number of seeding trials for each center (except the first),
        of which the one reducing inertia the most is greedily chosen.
        Set to None to make the number of trials depend logarithmically
        on the number of seeds (2+log(k)); this is the default.

    Notes
    -----
    Selects initial cluster centers for k-mean clustering in a smart way
    to speed up convergence. see: Arthur, D. and Vassilvitskii, S.
    "k-means++: the advantages of careful seeding". ACM-SIAM symposium
    on Discrete algorithms. 2007

    Version ported from http://www.stanford.edu/~darthur/kMeansppTest.zip,
    which is the implementation used in the aforementioned paper.
    """
    n_samples, n_features = X.shape

    centers = np.empty((n_clusters, n_features), dtype=X.dtype)

    assert x_squared_norms is not None, 'x_squared_norms None in _k_init'

    # Set the number of local seeding trials if none is given
    if n_local_trials is None:
        # This is what Arthur/Vassilvitskii tried, but did not report
        # specific results for other than mentioning in the conclusion
        # that it helped.
        n_local_trials = 2 + int(np.log(n_clusters))

    # Pick first center randomly
    center_id = random_state.randint(n_samples)
    if sp.issparse(X):
        centers[0] = X[center_id].toarray()
    else:
        centers[0] = X[center_id]

    # Initialize list of closest distances and calculate current potential
    closest_dist_sq = euclidean_distances(
        centers[0, np.newaxis], X, Y_norm_squared=x_squared_norms,
        squared=True)
    current_pot = closest_dist_sq.sum()

    # Pick the remaining n_clusters-1 points
    for c in range(1, n_clusters):
        # Choose center candidates by sampling with probability proportional
        # to the squared distance to the closest existing center
        rand_vals = random_state.random_sample(n_local_trials) * current_pot
        candidate_ids = np.searchsorted(closest_dist_sq.cumsum(), rand_vals)

        # Compute distances to center candidates
        distance_to_candidates = euclidean_distances(
            X[candidate_ids], X, Y_norm_squared=x_squared_norms, squared=True)

        # Decide which candidate is the best
        best_candidate = None
        best_pot = None
        best_dist_sq = None
        for trial in range(n_local_trials):
            # Compute potential when including center candidate
            new_dist_sq = np.minimum(closest_dist_sq,
                                     distance_to_candidates[trial])
            new_pot = new_dist_sq.sum()

            # Store result if it is the best local trial so far
            if (best_candidate is None) or (new_pot < best_pot):
                best_candidate = candidate_ids[trial]
                best_pot = new_pot
                best_dist_sq = new_dist_sq

        # Permanently add best center candidate found in local tries
        if sp.issparse(X):
            centers[c] = X[best_candidate].toarray()
        else:
            centers[c] = X[best_candidate]
        current_pot = best_pot
        closest_dist_sq = best_dist_sq

    return centers


def _validate_center_shape(X, n_centers, centers):
    """Check if centers is compatible with X and n_centers"""
    if len(centers) != n_centers:
        raise ValueError('The shape of the initial centers (%s) '
                         'does not match the number of clusters %i'
                         % (centers.shape, n_centers))
    if centers.shape[1] != X.shape[1]:
        raise ValueError(
            "The number of features of the initial centers %s "
            "does not match the number of features of the data %s."
            % (centers.shape[1], X.shape[1]))


def _init_centroids(X, k, init='k-means++', random_state=None, x_squared_norms=None,
                    init_size=None):
    """Compute the initial centroids

    Parameters
    ----------

    X: array, shape (n_samples, n_features)

    k: int
        number of centroids

    init: {'k-means++', 'random' or ndarray or callable} optional
        Method for initialization

    random_state: integer or numpy.RandomState, optional
        The generator used to initialize the centers. If an integer is
        given, it fixes the seed. Defaults to the global numpy random
        number generator.

    x_squared_norms:  array, shape (n_samples,), optional
        Squared euclidean norm of each data point. Pass it if you have it at
        hands already to avoid it being recomputed here. Default: None

    init_size : int, optional
        Number of samples to randomly sample for speeding up the
        initialization (sometimes at the expense of accuracy): the
        only algorithm is initialized by running a batch KMeans on a
        random subset of the data. This needs to be larger than k.

    Returns
    -------
    centers: array, shape(k, n_features)
    """
    random_state = check_random_state(random_state)
    n_samples = X.shape[0]
    x_squared_norms = row_norms(X, squared=True)

    if isinstance(init, string_types) and init == 'k-means++':
        centers = _k_init(X, k, random_state=random_state,
                          x_squared_norms=x_squared_norms)
    elif isinstance(init, string_types) and init == 'random':
        seeds = random_state.permutation(n_samples)[:k]
        centers = X[seeds]
    elif hasattr(init, '__array__'):
        # ensure that the centers have the same dtype as X
        # this is a requirement of fused types of cython
        centers = np.array(init, dtype=X.dtype)
    elif callable(init):
        centers = init(X, k, random_state=random_state)
        centers = np.asarray(centers, dtype=X.dtype)
    else:
        raise ValueError("the init parameter for the k-means should "
                         "be 'k-means++' or 'random' or an ndarray, "
                         "'%s' (type '%s') was passed." % (init, type(init)))

    if sp.issparse(centers):
        centers = centers.toarray()

    _validate_center_shape(X, k, centers)
    return centers
