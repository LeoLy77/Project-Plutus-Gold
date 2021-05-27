import numpy as np
from sklearn.cluster import DBSCAN
from sklearn import metrics
from sklearn.datasets import make_blobs
from sklearn.preprocessing import StandardScaler
from sklearn.neighbors import NearestNeighbors
import matplotlib.pyplot as plt
from numpy.linalg import inv

class Point:
    def __init__(self, x, y) -> None:
        self.x = x
        self.y = y
        self.neigh = NearestNeighbors(n_neighbors=1)
    
    def update_pos2closest(self, pos_list, dstn_lim=0.5):
        """
        Finds the closest position in the list and update the current position\n
        Remains if the closest is too far
        """
        self.neigh.fit(pos_list)
        ret = self.neigh.kneighbors(np.array([self.x, self.y]).reshape(-1, 2), return_distance=True)
        dist = ret[0][0][0]
        if dist >= dstn_lim: #no update if distance is large
            return self.x, self.y
        new_pos_index = ret[1][0][0]
        x, y = pos_list[new_pos_index]
        self.x = x
        self.y = y
        # print("New position = ({a}, {b})".format(a=self.x, b=self.y))
        return x, y

class Cluster:
    def __init__(self, X, eps=0.25, min_samples=2) -> None:
        """
        X = [array(x, y), ...]\n
        @ret: {cl}
        """
        self.X = X
        X_scaled = StandardScaler().fit_transform(X)
        db = DBSCAN(eps=eps, min_samples=min_samples).fit(X_scaled) #resolution = 15cm, group of at least 5
        labels = db.labels_
        self.n_clusters_ = len(set(labels)) - (1 if -1 in labels else 0) # Number of clusters in labels, ignoring noise if present.
        self.n_noise_ = list(labels).count(-1)
        self.cluster_res = {i : [] for i in set(labels)}

        self.labels = labels
        self.cluster = db
        for i, cluster in enumerate(labels):
            self.cluster_res[cluster].append(X[i])

    def get_centers(self):
        """
        """
        res = []
        for key in self.cluster_res:
            if key == -1:
                continue #outliers
            split = list(zip(*self.cluster_res[key]))
            Xs = split[0]
            Ys = split[-1]
            res.append((np.mean(Xs), np.mean(Ys)))
        return res
    
    def print_stuff(self):
        for key in self.cluster_res:
            print(key, self.cluster_res[key])

        print('Estimated number of clusters: %d' % self.n_clusters_)
        print('Estimated number of noise points: %d' % self.n_noise_)
        print("Homogeneity: %0.3f" % metrics.homogeneity_score(labels_true, self.labels))
        print("Completeness: %0.3f" % metrics.completeness_score(labels_true, self.labels))
        print("V-measure: %0.3f" % metrics.v_measure_score(labels_true, self.labels))
        print("Adjusted Rand Index: %0.3f"
            % metrics.adjusted_rand_score(labels_true, self.labels))
        print("Adjusted Mutual Information: %0.3f"
            % metrics.adjusted_mutual_info_score(labels_true, self.labels))
        # print("Silhouette Coefficient: %0.3f"
        #     % metrics.silhouette_score(X, labels))

    # Plot result
    def plot(self, fig=None):
        if not fig:
            fig = plt
        core_samples_mask = np.zeros_like(self.labels, dtype=bool)
        core_samples_mask[self.cluster.core_sample_indices_] = True
        # Black removed and is used for noise instead.
        unique_labels = set(self.labels)
        colors = [fig.cm.Spectral(each)
                for each in np.linspace(0, 1, len(unique_labels))]

        coor = self.get_centers()
        for c in coor:
            fig.annotate("(%.3f, %.3f)" % (c[0], c[-1]), xy=c)

        for i, k, col in zip(range(len(colors)), unique_labels, colors):
            if k == -1:
                # Black used for noise.
                col = [0, 0, 0, 1]

            class_member_mask = (self.labels == k)

            xy = self.X[class_member_mask & core_samples_mask]
            if k == -1:
                fig.plot(xy[:, 0], xy[:, 1], 'o', markerfacecolor=tuple(col),
                    markeredgecolor='k', markersize=14, label="Noise")
            else:
                fig.plot(xy[:, 0], xy[:, 1], 'o', markerfacecolor=tuple(col),
                        markeredgecolor='k', markersize=14, label="Obj %d (%.3f, %.3f)" % (k, coor[i][0], coor[i][-1]))

            xy = self.X[class_member_mask & ~core_samples_mask]
            fig.plot(xy[:, 0], xy[:, 1], 'o', markerfacecolor=tuple(col),
                    markeredgecolor='k', markersize=6)

        fig.title('Estimated number of clusters: %d' % self.n_clusters_, fontsize=30)
        fig.legend(loc=3, bbox_to_anchor=(-0.12, -0.15))

def Kalman():
    x_observations = np.array([4000, 4260, 4550, 4860, 5110])
    v_observations = np.array([280, 282, 285, 286, 290])

    z = np.c_[x_observations, v_observations]

    # Initial Conditions
    a = 2  # Acceleration
    v = 280
    t = 1  # Difference in time

    # Process / Estimation Errors
    error_est_x = 20
    error_est_v = 5

    # Observation Errors
    error_obs_x = 25  # Uncertainty in the measurement
    error_obs_v = 6

    def prediction2d(x, v, t, a):
        A = np.array([[1, t],
                    [0, 1]])
        X = np.array([[x],
                    [v]])
        B = np.array([[0.5 * t ** 2],
                    [t]])
        X_prime = A.dot(X) + B.dot(a)
        return X_prime


    def covariance2d(sigma1, sigma2):
        cov1_2 = sigma1 * sigma2
        cov2_1 = sigma2 * sigma1
        cov_matrix = np.array([[sigma1 ** 2, cov1_2],
                            [cov2_1, sigma2 ** 2]])
        return np.diag(np.diag(cov_matrix))


    # Initial Estimation Covariance Matrix
    P = covariance2d(error_est_x, error_est_v)
    A = np.array([[1, t],
                [0, 1]])

    # Initial State Matrix
    X = np.array([[z[0][0]],
                [v]])
    n = len(z[0])

    for data in z[1:]:
        X = prediction2d(X[0][0], X[1][0], t, a)
        # set off-diagonal terms to 0.
        P = np.diag(np.diag(A.dot(P).dot(A.T)))

        # Calculating the Kalman Gain
        H = np.identity(n)
        R = covariance2d(error_obs_x, error_obs_v)
        S = H.dot(P).dot(H.T) + R
        K = P.dot(H).dot(inv(S))

        # Reshape the new data into the measurement space.
        Y = H.dot(data).reshape(n, -1)

        # Update the State Matrix
        # Combination of the predicted state, measured values, covariance matrix and Kalman Gain
        X = X + K.dot(Y - H.dot(X))

        # Update Process Covariance Matrix
        P = (np.identity(len(K)) - K.dot(H)).dot(P)

        print("Kalman Filter State Matrix:\n", X)

if __name__ == "__main__":
    centers = [[1, 1], [-1, -1], [1, -1], [0, 0]]     # Generate sample data
    X, labels_true = make_blobs(n_samples=50, centers=centers, cluster_std=0.1,
                                random_state=0)
    X = np.around(X,decimals=6)
    print(list(X[:, 0]))
    print(list(X[:, 1]))
    cluster = Cluster(X)
    cluster.plot()
    print(cluster.get_centers())

    #test moving object
    # _, axs = plt.subplots(1, 1)
    # o = Point(1,1)
    # o1 = Point(-1,-1)
    # for i in range(10):
    #     grp = X[i*10 : i*10+10]
    #     x, y = o.update_pos2closest(grp)
    #     axs.scatter(x, y, c="red")
    #     x, y = o1.update_pos2closest(grp)
    #     axs.scatter(x, y, c="blue")

    plt.show()
