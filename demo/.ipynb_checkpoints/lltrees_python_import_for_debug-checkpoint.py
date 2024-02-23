import sys
sys.path.append('/home/alexandre/Desktop/lltrees/src/build')

import sklearn.datasets, sklearn.metrics, sklearn.model_selection, sklearn.tree
X, Y = sklearn.datasets.make_regression(n_samples=1000, n_features=8, n_informative=5, n_targets=1, noise=1, random_state=42)
X_train, X_test, Y_train, Y_test = sklearn.model_selection.train_test_split(X, Y, test_size=0.3, random_state=42)

import lltrees
my_lltree = lltrees.lltree()
my_lltree.fit(X_train, Y_train)
YP = my_lltree.predict(X_test)
print(YP[0 : 10])
print(sum(abs(YP - Y_test)) / len(YP))
my_lltree.test()
residuals = my_lltree.get_residuals()
print(residuals)