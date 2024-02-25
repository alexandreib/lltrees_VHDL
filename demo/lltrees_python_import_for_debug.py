import time

import sys
sys.path.append('/home/alexandre/Desktop/lltrees/src/build')

import sklearn.datasets, sklearn.metrics, sklearn.model_selection, sklearn.tree
X, Y = sklearn.datasets.make_regression(n_samples=1000, n_features=8, n_informative=5, n_targets=1, noise=1, random_state=42)
X_train, X_test, Y_train, Y_test = sklearn.model_selection.train_test_split(X, Y, test_size=0.3, random_state=42)


import lltrees
conf ={
    'epochs' : 1,
    'learning_rate' : 1,
    'metric_name' : 'mae', # Possible metrics for the moment : mae, mse
    'lltree_max_depth' : 5,
    'lltree_min_size_split' : 2,
    'lltree_criterion' : "absolute_error",  # Possible metrics for the moment : variance, absolute_error
}
my_lltree = lltrees.lltree()
my_lltree.set_conf(conf)
my_lltree.get_conf()

start_time = time.time()
my_lltree.fit(X_train, Y_train, X_test, Y_test)
YP = my_lltree.predict(X_test)
print("--- %s seconds ---" % (time.time() - start_time))

residuals = my_lltree.get_residuals()

conf ={
    'epochs' : 50,
    'learning_rate' : 0.1,
    'metric_name' : 'mae', # Possible metrics for the moment : mae, mse
    'lltree_max_depth' : 3,
    'lltree_min_size_split' : 1,
    'lltree_criterion' : "variance",  # Possible metrics for the moment : variance, absolute_error
}
my_lltree.set_conf(conf)
my_lltree.get_conf()

start_time = time.time()
my_lltree.fit(X_train, Y_train, X_test, Y_test)
YP = my_lltree.predict(X_test)
print("--- %s seconds ---" % (time.time() - start_time))

residuals = my_lltree.get_residuals()


print('PYTHON')
print(YP[0 : 10])
print(sum(abs(YP - Y_test)) / len(YP))

print(residuals)