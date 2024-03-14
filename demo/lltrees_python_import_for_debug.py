import time
import numpy as np

import sklearn.datasets, sklearn.metrics, sklearn.model_selection, sklearn.tree
X, Y = sklearn.datasets.make_regression(n_samples=1000, n_features=9, n_informative=5, n_targets=1, noise=1, random_state=42)
X_train, X_test, Y_train, Y_test = sklearn.model_selection.train_test_split(X, Y, test_size=0.3, random_state=42)

# print(X_train[:2,:2])
# print(Y_train[:5])
import sys
sys.path.append('/home/alexandre/Desktop/lltrees/build')
import lltrees
conf ={
    'mode' : 'regression',
    'epochs' : 10,
    'learning_rate' : 0.1,
    'algo_name' : 'test',
    'max_depth' : 5,
    'min_leaf_size' :1,
    'verbose' : 1, 
    'metric' : 'mae', 
    'criterion' : "absolute_error", 
}
my_lltree = lltrees.lltree()
my_lltree.set_conf(conf)
my_lltree.get_conf()

start_time = time.time()
my_lltree.fit(X_train, Y_train, X_test, Y_test)
print("FIT --- %s seconds ---" % (time.time() - start_time))
start_time = time.time()
YP = my_lltree.predict(X_test)
print("PREDICT --- %s seconds ---" % (time.time() - start_time))
print(my_lltree.get_residuals())
print("rmse: %.2f" % np.sqrt(sklearn.metrics.mean_squared_error(Y_test,YP)))
print("mae: %.2f" % sklearn.metrics.mean_absolute_error(Y_test,YP))
print("r2: %.2f" % sklearn.metrics.r2_score(Y_test,YP))

my_lltree.save()
# del my_lltree
# my_lltree = lltrees.lltree()
# my_lltree.set_conf(conf)
# my_lltree.get_conf()

# start_time = time.time()
# my_lltree.fit(X_train, Y_train, X_test, Y_test)
# print("FIT --- %s seconds ---" % (time.time() - start_time))
# start_time = time.time()
# YP = my_lltree.predict(X_test)
# print("PREDICT --- %s seconds ---" % (time.time() - start_time))
# print(my_lltree.get_residuals())

# print("rmse: %.2f" % np.sqrt(sklearn.metrics.mean_squared_error(Y_test,YP)))
# print("mae: %.2f" % sklearn.metrics.mean_absolute_error(Y_test,YP))
# print("r2: %.2f" % sklearn.metrics.r2_score(Y_test,YP))
# print(YP[0:10])