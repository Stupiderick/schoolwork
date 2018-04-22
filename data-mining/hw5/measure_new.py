import numpy as np
import pandas as pd
import sys

def erickslog2(df):
    '''
    return 0 for log2(0),
    otherwise np.log2()
    '''
    return np.log2(df.replace(0.0, 1.0))

def get_scores(df_a):
    '''
    Input:
    df_a: a dataframe that takes two columns:
        attr   | label
        ------------------
        (cat)  | (target)
        (cat)  | (target)
    -------
    Return:
    tuple(Information gain, gain ratio, gini index)
    '''
    attr = list(df_a)[0]
    label = list(df_a)[1]
    tuple_ct = df_a.shape[0]
    target_list = df_a[label].unique().tolist()
    column_list = [attr] + target_list
    info_table = pd.DataFrame(0, index=np.arange(0), columns=column_list)
    cats = df_a[attr].unique()
    groupie = df_a.groupby([attr, label]).size().reset_index(name='counts')

    for cat in cats:
        newbie = pd.DataFrame([[cat] + [0] * (len(column_list)-1)], columns=column_list)
        for target in target_list:
            if df_a[(df_a[attr]==cat) & (df_a[label]==target)].count(1).size != 0:
                newbie[target] = groupie[(groupie[attr]==cat) & (groupie[label]==target)]['counts'].tolist()[0]
            else:
                newbie[target] = 0
        info_table = info_table.append((newbie), ignore_index=True)

    # Calculate info(D_j), splitInfo(D_j), and Gini(D_j):
    summation = info_table[target_list].sum(axis=1)
    DjD = (info_table[target_list].T / summation.T).T

    ct = info_table[target_list].sum(axis=0)

    info = 0.0 - (ct / tuple_ct * erickslog2(ct/tuple_ct)).sum(axis=0)
    info_a = (0.0 - summation / tuple_ct * (DjD * erickslog2(DjD)).sum(axis=1)).sum()
    ig = info - info_a

    # Calculate gain ratio:
    split_info_a = (0.0 - summation / tuple_ct * erickslog2(summation / tuple_ct)).sum()
    gr = 0.0
    if split_info_a != 0.0:
        gr = ig / split_info_a
    else:
        gr = ig

    # Calculate gini index:
    gini = 1.0 - (np.square(ct/tuple_ct)).sum(axis=0)
    gini_a = (summation / tuple_ct * (1.0 - (np.square(DjD)).sum(axis=1))).sum()
    gi = gini - gini_a

    return ig, gr, gi

def loopie(i):
    sub_df = df.take([i, len(df.columns)-1], axis=1)
    info_gain, gain_ratio, gini_index = get_scores(sub_df)
    ig_list.append(info_gain)
    gr_list.append(gain_ratio)
    gi_list.append(gini_index)
    return

def readstuff(line):
    line = line.replace('\n', '')
    raw.append(line)

raw = []
[readstuff(line) for line in sys.stdin]

setsize = int(raw[0])
attr_list = raw[1].split(',')
data = raw[1:]
df = pd.DataFrame([feature.split(',') for feature in data])
df = df.rename(columns=df.iloc[0]).drop(df.index[0])

ig_list = []
gr_list = []
gi_list = []

[loopie(i) for i in range(len(df.columns)-1)]
print(attr_list[ig_list.index(max(ig_list))])
print(attr_list[gr_list.index(max(gr_list))])
print(attr_list[gi_list.index(max(gi_list))], end='')
