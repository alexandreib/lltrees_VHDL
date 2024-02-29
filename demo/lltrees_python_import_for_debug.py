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
    'algo_name' : 'test',
    'max_depth' : 5,
    'min_leaf_size' : 1,
    
    'mode' : "regression", 
    'verbose' : 1, 
    'metric' : 'mae', 
    'criterion' : "absolute_error", 
}
my_lltree = lltrees.lltree()
# my_lltree.save()
my_lltree.set_conf(conf)
my_lltree.get_conf()

print(X_train[0,0])
start_time = time.time()
my_lltree.fit(X_train, Y_train)
print("FIT --- %s seconds ---" % (time.time() - start_time))

start_time = time.time()
# YP = my_lltree.predict(X_test)
YP = my_lltree.predict(X_train)
print("PREDICT --- %s seconds ---" % (time.time() - start_time))

print(YP[0:10])
# conf ={
#     'epochs' : 50,
#     'learning_rate' : 0.1,
#     'boost_algo_name' : 'test',
#     'max_depth' : 2,
#     'min_leaf_size' : 1,
#     'mode' : "regression", 
#     'verbose' : 1, 
#     'metric' : 'mae', 
#     'criterion' : "variance", 
# }

# my_lltree = lltrees.lltree()
# my_lltree.set_conf(conf)
# my_lltree.get_conf()

# start_time = time.time()
# my_lltree.fit(X_train, Y_train, X_test, Y_test)
# print("FIT --- %s seconds ---" % (time.time() - start_time))

# start_time = time.time()
# YP = my_lltree.predict(X_test)
# print("PREDICT --- %s seconds ---" % (time.time() - start_time))

# print(YP[0:3])

# X, Y = sklearn.datasets.make_classification(n_samples=1000, n_features=8, n_informative=5, n_classes=2, random_state=42)
# X_train, X_test, Y_train, Y_test = sklearn.model_selection.train_test_split(X, Y, test_size=0.3, random_state=42)

# conf ={
#     'epochs' : 50,
#     'learning_rate' : 0.1,
#     'algo_name' : 'test',
#     'metric' : 'accuracy', 
#     'max_depth' : 2,
#     'min_leaf_size' : 1,
#     'criterion' : "gini", 
#     'mode' : "classification", 
#     'verbose' : 1, 
# }

# my_lltree = lltrees.lltree()
# my_lltree.set_conf(conf)
# my_lltree.get_conf()

# start_time = time.time()
# my_lltree.fit(X_train, Y_train)
# print("FIT --- %s seconds ---" % (time.time() - start_time))

# start_time = time.time()
# YP = my_lltree.predict(X_test)
# print("PREDICT --- %s seconds ---" % (time.time() - start_time))

# print(YP)
# print("accuracy_score: %.2f" % sklearn.metrics.accuracy_score(Y_test,YP))
# print("log_loss: %.2f" % sklearn.metrics.log_loss(Y_test,YP))
# print("f1_score: %.2f" % sklearn.metrics.f1_score(Y_test,YP))