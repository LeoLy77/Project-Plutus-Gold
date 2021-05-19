import numpy as np
from sklearn.cluster import DBSCAN
from sklearn import metrics
from sklearn.datasets import make_blobs
from sklearn.preprocessing import StandardScaler
import matplotlib.pyplot as plt

class Cluster:
    def __init__(self, X) -> None:
        """
        X = [array(x, y), ...]\n
        @ret: {cl}
        """
        X_scaled = StandardScaler().fit_transform(X)
        db = DBSCAN(eps=0.1, min_samples=5).fit(X_scaled) #resolution = 10cm, group of at least 5
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
    def plot(self):
        core_samples_mask = np.zeros_like(self.labels, dtype=bool)
        core_samples_mask[self.cluster.core_sample_indices_] = True
        # Black removed and is used for noise instead.
        unique_labels = set(self.labels)
        colors = [plt.cm.Spectral(each)
                for each in np.linspace(0, 1, len(unique_labels))]
        for k, col in zip(unique_labels, colors):
            if k == -1:
                # Black used for noise.
                col = [0, 0, 0, 1]

            class_member_mask = (self.labels == k)

            xy = X[class_member_mask & core_samples_mask]
            plt.plot(xy[:, 0], xy[:, 1], 'o', markerfacecolor=tuple(col),
                    markeredgecolor='k', markersize=14)

            xy = X[class_member_mask & ~core_samples_mask]
            plt.plot(xy[:, 0], xy[:, 1], 'o', markerfacecolor=tuple(col),
                    markeredgecolor='k', markersize=6)

        plt.title('Estimated number of clusters: %d' % self.n_clusters_)

if __name__ == "__main__":
    # Generate sample data
    centers = [[1, 1], [-1, -1], [1, -1]]
    X, labels_true = make_blobs(n_samples=100, centers=centers, cluster_std=0.1,
                                random_state=0)

    cluster = Cluster(X)
    cluster.plot()
    print(cluster.get_centers())

    plt.show()