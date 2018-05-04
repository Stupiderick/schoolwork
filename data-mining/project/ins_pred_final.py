import numpy as np
import pandas as pd


def split(feat, val, X_y):
    lefthand = np.array([]).reshape(0, 80)
    righthand = np.array([]).reshape(0, 80)
    for i in X_y:
        if i[feat] <= val:
            lefthand = np.vstack((lefthand,i))
        if i[feat] > val:
            righthand = np.vstack((righthand,i))
    return lefthand, righthand


def gini_score(gp, cl):
    n_samples = sum([len(group) for group in gp])
    gini = 0
    for group in gp:
        size = float(len(group))
        if size == 0:
            continue
        score = 0.0
        for class_val in cl:
            p = (group[:,-1] == class_val).sum() / size
            score += p ** 2
        gini += (1.0 - score) * (size / n_samples)
    return gini


def best_split(X_y, loop=None):
    cl = np.unique(X_y[:,-1])
    b_feat = b_val = b_score = 999
    b_gp = 0
    count = X_y.shape[1]-1
    for feat in range(X_y.shape[1]-1):
        if loop == None and feat % 10 == 0:
            print('split_treeBranch: ' + str(count) + ' to go.')
        elif loop != None:
            print('Best_split: ' + str(count) + ' to go. In loop ' + str(loop+1) + '.')
        count -= 1

        for i in X_y:
            gp = split(feat, i[feat], X_y)
            gini = gini_score(gp, cl)
            if gini < b_score:
                b_feat = feat
                b_val = i[feat]
                b_score = gini
                b_gp = gp

    ret = dict()
    ret['feat'] = b_feat
    ret['val'] = b_val
    ret['gp'] = b_gp
    return ret


def end_node(group):
    cl, counts = np.unique(group[:,-1], return_counts=True)
    return cl[np.argmax(counts)]


def split_treeBranch(node, max_depth, min_sample, depth):
    l_node, r_node = node['gp']

    if not isinstance(l_node, np.ndarray) or not isinstance(r_node, np.ndarray):
        node['left'] = node['right'] = end_node(l_node + r_node)
        return

    if depth >= max_depth:
        node['left'] = end_node(l_node)
        node['right'] = end_node(r_node)
        return

    if len(r_node) <= min_sample:
        node['right'] = end_node(r_node)
    else:
        node['right'] = best_split(r_node)
        split_treeBranch(node['right'], max_depth, min_sample, depth+1)

    if len(l_node) <= min_sample:
        node['left'] = end_node(l_node)
    else:
        node['left'] = best_split(l_node)
        split_treeBranch(node['left'], max_depth, min_sample, depth+1)


def build_tree(X_y, max_depth, min_sample, i):
    root = best_split(X_y, loop=i)
    split_treeBranch(root, max_depth, min_sample, 1)
    return root


def sample_pred(node, sample):
    if sample[node['feat']] < node['val']:
        if isinstance(node['left'], dict):
            return sample_pred(node['left'], sample)
        else:
            return node['left']
    else:
        if isinstance(node['right'], dict):
            return sample_pred(node['right'], sample)
        else:
            return node['right']


def predict(X):
    y_pred = np.array([])
    for i in X:
        y_pred = np.append(y_pred, sample_pred(tree,i))
    return y_pred.astype('int')


def accuracy(pred, given):
    correct = 0
    for i in range(len(pred)):
        if pred[i] == given[i]:
            correct += 1
    return correct / len(pred)

data_test = np.array(pd.read_csv('testing.csv'))
data = np.array(pd.read_csv('training.csv'))

for i in range(len(data[:, 2])):
    if pd.isnull(data[i, 2]) == False:
        data[i, 2] = float(data[i, 2][1])

for i in range(len(data_test[:, 2])):
    if pd.isnull(data_test[i, 2]) == False:
        data_test[i, 2] = float(ord(data_test[i, 2][0])-65) * 10.0 + float(data_test[i, 2][1])

X_train_full = data[:, 1:80].astype('float')

for j in range(X_train_full.shape[1]):
    mean = np.nanmean(X_train_full[:, j], axis=0)
    # X_train[:,i][pd.isnull(X_train[:,i])] = mean
    for k in range(X_train_full.shape[0]):
        if pd.isnull(X_train_full[k][j]):
            X_train_full[k][j] = mean

y_test_array = 0
whole_array = 0
for i in range(19):
    indices = np.random.randint(0, data.shape[0], 200)
    X_train = X_train_full[indices, :].astype('float')
    X_test = data_test[:, 1:80].astype('float')
    # X_train = data[:500, 3:80].astype('float')

    a = pd.isnull(X_train)
    b = np.sum(a)
    print('b', b)

    # y_train = data[:500, -1].astype('int')
    y_train = data[indices, -1].astype('int')

    X = X_train
    y = y_train
    print('X_test', X, X.shape)
    print('y_test', y, y.shape)

    X_y = np.column_stack((X, y))
    tree = build_tree(X_y, 6, 50, i)
    y_pred = predict(X)
    # y_nons_pred = predict(data[401:, 3:80].astype('float'))
    # y_nons = data[401:, -1].astype('int')
    y_whole_pred_raw = predict(data[:, 1:80].astype('float'))
    y_whole_pred = np.reshape(y_whole_pred_raw, (1, y_whole_pred_raw.shape[0]))
    y_whole = data[:, -1].astype('int')
    y_test_pred = predict(X_test)
    y_test_pred = np.reshape(y_test_pred, (1, y_test_pred.shape[0]))
    # print('f', y_test_pred)
    if i == 0:
        y_test_array = y_test_pred
        whole_array = y_whole_pred
    else:
        y_test_array = np.concatenate((y_test_array, y_test_pred), axis=0)
        whole_array = np.concatenate((whole_array, y_whole_pred), axis=0)

    print('sample', i, accuracy(y_pred, y))
    # print('non-sample', accuracy(y_nons_pred, y_nons))
    print('whole', i, accuracy(y_whole_pred_raw, y_whole))

y_test_array = y_test_array.astype('int')
whole_array = whole_array.astype('int')

y_test_df = pd.DataFrame(y_test_array)
# y_test_array.to_csv('testing_loops.csv', index=False, header=False)

whole_df = pd.DataFrame(whole_array)
# whole_array.to_csv('training_loops.csv', index=False, header=False)

np.savetxt('y_result.csv', y_test_array.astype('int'), delimiter=',')
np.savetxt('whole.csv', whole_array.astype('int'), delimiter=',')

result = y_test_array.T
whole = whole_array.T

y_test_pred_mf = np.array([])
whole_pred_mf = np.array([])

for row in result:
    counts = np.bincount(row)
    r = np.argmax(counts)
    y_test_pred_mf = np.append(y_test_pred_mf, r)

for row in whole:
    counts = np.bincount(row)
    r = np.argmax(counts)
    whole_pred_mf = np.append(whole_pred_mf, r)

print('whole_full', i, accuracy(whole_pred_mf, y_whole))

# output prediction
y_pred_test = np.reshape(y_test_pred_mf, (y_test_pred_mf.shape[0],1))
y_ID = data_test[:,0]
y_ID = np.reshape(y_ID, (y_ID.shape[0],1))
y = np.append(y_ID, y_pred_test, axis=1).astype('int')

Title = np.array(['Id','Response'])
Title = np.reshape(Title, (1,2))
y = np.append(Title, y, axis=0)

my_df = pd.DataFrame(y)
my_df.to_csv('out.csv', index=False, header=False)
