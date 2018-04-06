from sklearn import multiclass, svm
import numpy as np
import pandas as pd
from sklearn import metrics
from sklearn.svm import LinearSVC
from sklearn.multiclass import OneVsRestClassifier
from sklearn.multiclass import OneVsOneClassifier
import sklearn as sk
import csv

def sklearn_multiclass_prediction(mode, X_train, y_train, X_test):
    '''
    Use Scikit Learn built-in functions multiclass.OneVsRestClassifier
    and multiclass.OneVsOneClassifier to perform multiclass classification.

    Arguments:
        mode: one of 'ovr', 'ovo' or 'crammer'.
        X_train, X_test: numpy ndarray of training and test features.
        y_train: labels of training data, from 0 to 9.

    Returns:
        y_pred_train, y_pred_test: a tuple of 2 numpy ndarrays,
                                   being your prediction of labels on
                                   training and test data, from 0 to 9.

    [we grabbed this func from sklearn, and modified it a little bit]
    '''
    if mode == 'crammer':
        clf = LinearSVC(random_state=12345, multi_class='crammer_singer')
    elif mode == 'ovo':
        clf = OneVsOneClassifier(LinearSVC(degree=3, random_state=12345))
    else:
        clf = sk.linear_model.LogisticRegression(max_iter=100)


    clf.fit(X_train, y_train)
    y_pred_train = clf.predict(X_train)
    y_pred_test = clf.predict(X_test)
    return (y_pred_train, y_pred_test)


data = pd.read_csv('training.csv')
# convert the dataframe to numpy array.
data = np.array(data)

# dump the first column which is id.
X_train = data[:,1:80]

# insert a space for characters
X_train = np.insert(X_train, 1, np.NaN, axis=1)

# convert '[char][int]' into ints.
for i in range(X_train.shape[0]):
    if pd.isnull(X_train[i][2]) == False:
        string = X_train[i][2]
        X_train[i][1] = float(ord(string[0]) - 65.0)
        X_train[i][2] = float(string[1])

X_train = X_train.astype('float')

# all NaNs into averages.
for j in range(X_train.shape[1]):
    mean = np.nanmean(X_train[:,j], axis=0)
    for i in range(X_train.shape[0]):
        if pd.isnull(X_train[i][j]):
            X_train[i][j] = mean

print('training data', X_train)

a = pd.isnull(X_train)
b = np.sum(a)

# calculating testing set
y_train = data[:,-1].astype('int')

data2 = pd.read_csv('testing.csv')
data2 = np.array(data2)

X_test = data2[:,1:80]

X_test = np.insert(X_test, 1, np.NaN, axis=1)
for i in range(X_test.shape[0]):
    if pd.isnull(X_test[i][2]) == False:
        string = X_test[i][2]
        X_test[i][1] = float(ord(string[0]) - 65.0)
        X_test[i][2] = float(string[1])

X_test = X_test.astype('float')

for j in range(X_test.shape[1]):
    mean = np.nanmean(X_test[:,j], axis=0)
    for i in range(X_test.shape[0]):
        if pd.isnull(X_test[i][j]):
            X_test[i][j] = mean

print('testing data', X_test)

y_pred_train, y_pred_test = sklearn_multiclass_prediction(
            'ovr', X_train, y_train, X_test)

train_acc = metrics.accuracy_score(y_train, y_pred_train)
print('accuracy', train_acc)

y_pred_test = np.reshape(y_pred_test, (y_pred_test.shape[0],1))
y_ID = data2[:,0]
y_ID = np.reshape(y_ID, (y_ID.shape[0],1))
y = np.append(y_ID, y_pred_test, axis=1)

Title = np.array(['Id','Response'])
Title = np.reshape(Title, (1,2))
y = np.append(Title,y,axis=0)

# output the result
my_df = pd.DataFrame(y)
my_df.to_csv('out.csv', index=False, header=False)

# get error in the training set
errorlist = y_train - y_pred_train
unique, counts = np.unique(errorlist, return_counts=True)
summation = float(np.sum(counts))
freqs = []
for i in range(len(counts)):
    freqs.append(float(counts[i]) / summation)
error_freq = dict(zip(unique, freqs))
print('{error: freqencies}', error_freq)
