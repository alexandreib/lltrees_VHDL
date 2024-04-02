import time
import numpy as np
import pandas as pd
import time
import sklearn.datasets, sklearn.metrics, sklearn.model_selection, sklearn.tree
from sklearn.datasets import load_breast_cancer

import sys
sys.path.append('/home/alexandre/Desktop/lltrees/build')

data = load_breast_cancer()
X    = data.data
y    = data.target

nsampclass = int(np.round(y.shape[0]*0.6/2,decimals=0))
y0 = y[y==0]
y1 = y[y==1]
X0 = X[y==0]
X1 = X[y==1]
# select the elements to remove at random
idx0 = np.random.choice([i for i in range(y0.shape[0])],size=nsampclass,replace=False)
idx1 = np.random.choice([i for i in range(y1.shape[0])],size=nsampclass,replace=False)
# select samples for training
y_train0 = y0[idx0]
y_train1 = y1[idx1]
X_train0 = X0[idx0,:]
X_train1 = X1[idx1,:]
y_train  = np.concatenate((y_train0,y_train1))
X_train  = np.concatenate((X_train0,X_train1))
# use remainder for testing
y_test0 = np.delete(y0,idx0)
y_test1 = np.delete(y1,idx1)
X_test0 = np.delete(X0,idx0,axis=0)
X_test1 = np.delete(X1,idx1,axis=0)
y_test  = np.concatenate((y_test0,y_test1))
X_test  = np.concatenate((X_test0,X_test1))

import lltrees
my_lltree = lltrees.lltree()

conf ={
    'mode' : 'classification',
    'epochs' : 1,
    'learning_rate' : 1,
    'metric' : 'accuracy',
    'max_depth' : 5,
    'min_leaf_size' : 2,
    'criterion' : "gini",  
    'verbose' : 1,  
}
my_lltree.set_conf(conf)
my_lltree.get_conf()
start_time = time.time()
my_lltree.fit(X_train, y_train, X_test, y_test)
print("FIT --- %s seconds ---" % (time.time() - start_time))

my_lltree.print()

start_time = time.time()
YP = my_lltree.predict(X_test)
print("PREDICT --- %s seconds ---" % (time.time() - start_time))

print("accuracy_score: %.2f" % np.sqrt(sklearn.metrics.accuracy_score(y_test,YP)))
print("log_loss: %.2f" % sklearn.metrics.log_loss(y_test,YP))
print("f1_score: %.2f" % sklearn.metrics.f1_score(y_test,YP))
print(np.unique(YP, return_counts=True))

############################################################
############################################################
############################################################
############################################################
############################################################
############################################################
############################################################
############################################################
############################################################
############################################################
X,y = sklearn.datasets.make_regression(n_samples=1000, n_features=8, n_informative=5, n_targets=1, noise=1, random_state=42)
X_train, X_test, y_train, y_test = sklearn.model_selection.train_test_split(X, y, test_size=0.3, random_state=42)

conf ={
    'mode' : 'regression',
    'epochs' : 1,
    'learning_rate' : 1,
    'max_depth' : 5,
    'min_leaf_size' : 1,
    'verbose' : 1, 
    'metric' : 'mae', 
    'criterion' : "absolute_error", 
}
my_lltree = lltrees.lltree()
my_lltree.set_conf(conf)
my_lltree.get_conf()
start_time = time.time()
my_lltree.fit(X_train, y_train, X_test, y_test)
print("FIT --- %s seconds ---" % (time.time() - start_time))
start_time = time.time()
YP = my_lltree.predict(X_test)
print("PREDICT --- %s seconds ---" % (time.time() - start_time))
print(my_lltree.get_residuals())
print("rmse: %.2f" % np.sqrt(sklearn.metrics.mean_squared_error(y_test,YP)))
print("mae: %.2f" % sklearn.metrics.mean_absolute_error(y_test,YP))
print("r2: %.2f" % sklearn.metrics.r2_score(y_test,YP))

############################################################
############################################################
############################################################
############################################################
############################################################
############################################################
############################################################
############################################################
############################################################
############################################################




X, Y = sklearn.datasets.make_classification(n_samples=1000, n_features=8, n_informative=5, n_classes=2, random_state=42)
X_train, X_test, Y_train, Y_test = sklearn.model_selection.train_test_split(X, Y, test_size=0.3, random_state=42)

import lltrees
my_lltree = lltrees.lltree()

conf ={
    'mode' : 'classification',
    'epochs' : 2,
    'learning_rate' : 0.5,
    'metric' : 'accuracy',
    'max_depth' : 2,
    'min_leaf_size' : 1,
    'criterion' : "gini",  
    'verbose' : 1,  
}

my_lltree.set_conf(conf)
my_lltree.get_conf()

start_time = time.time()
my_lltree.fit(X_train, Y_train, X_test, Y_test)
print("FIT --- %s seconds ---" % (time.time() - start_time))

my_lltree.print()

start_time = time.time()
YP = my_lltree.predict(X_test)
print("PREDICT --- %s seconds ---" % (time.time() - start_time))

print("accuracy_score: %.2f" % np.sqrt(sklearn.metrics.accuracy_score(Y_test,YP)))
print("log_loss: %.2f" % sklearn.metrics.log_loss(Y_test,YP))
print("f1_score: %.2f" % sklearn.metrics.f1_score(Y_test,YP))
print(np.unique(YP, return_counts=True))

YP = my_lltree.predict_proba(X_test)
df = pd.DataFrame(YP)
print(YP[:10])
print(df.head())
print(df.shape)

time.sleep(1)

X, Y = sklearn.datasets.make_regression(n_samples=1000, n_features=9, n_informative=5, n_targets=1, noise=1, random_state=42)
X_train, X_test, Y_train, Y_test = sklearn.model_selection.train_test_split(X, Y, test_size=0.3, random_state=42)


conf ={
    'mode' : 'regression',
    'epochs' : 3,
    'learning_rate' : 0.1,
    'max_depth' : 5,
    'min_leaf_size' : 2,
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
# my_lltree.print()


# del my_lltree
# my_lltree = lltrees.lltree()
# my_lltree.set_conf(conf)
# # my_lltree.get_conf()
# my_lltree.load()
# # my_lltree.print()

# # start_time = time.time()
# # my_lltree.fit(X_train, Y_train, X_test, Y_test)
# # print("FIT --- %s seconds ---" % (time.time() - start_time))
# start_time = time.time()
# YP = my_lltree.predict(X_test)
# print("PREDICT --- %s seconds ---" % (time.time() - start_time))
# print(my_lltree.get_residuals())

# print("rmse: %.2f" % np.sqrt(sklearn.metrics.mean_squared_error(Y_test,YP)))
# print("mae: %.2f" % sklearn.metrics.mean_absolute_error(Y_test,YP))
# print("r2: %.2f" % sklearn.metrics.r2_score(Y_test,YP))
# print(YP[0:10])