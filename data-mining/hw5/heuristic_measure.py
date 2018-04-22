import numpy as np
import pandas as pd
import sys
import math

def get_scores(df_a):
    '''
    Input:
    df_a: a dataframe that takes two columns:
        attr       | label
        ----------------------
        (feature)  | (target)
        (feature)  | (target)
    -------
    Return:
    tuple(Information gain, gain ratio, gini index)
    '''
    attr = list(df_a)[0]
    label = list(df_a)[1]
    tuple_ct = df_a.shape[0]
    column_list = [attr, 'YES', 'NO', 'INFO_w', 'SPLIT_w', 'GINI_w']

    info_table = pd.DataFrame(0, index=np.arange(0), columns=column_list)

    for index, row in df_a.iterrows():
        cat = row[attr]
        target = row[label]
        # If the values are not in the info table, then add them in...
        if cat not in info_table[attr].tolist():
            if target == 'yes':
                newbie = pd.DataFrame([[cat, 1, 0, 0, 0, 0]], columns=column_list)
            else:
                newbie = pd.DataFrame([[cat, 0, 1, 0, 0, 0]], columns=column_list)
            info_table = info_table.append((newbie), ignore_index=True)
        # If they have existed in the table, increment the required field...
        else:
            found_index = info_table[attr][info_table[attr] == cat].index[0]
            if target == 'yes':
                info_table.ix[found_index, 'YES'] += 1
            else:
                info_table.ix[found_index, 'NO'] += 1

    # Calculate info(D_j), splitInfo(D_j), and Gini(D_j):
    for index, row in info_table.iterrows():
        yes = row['YES']
        no = row['NO']
        total = yes + no
        if yes != 0 and no != 0:
            info_table.ix[index, 'INFO_w'] = total / tuple_ct * (0.0 - yes / total * np.log2(yes/total) - no / total * np.log2(no/total))
        elif yes == 0 and no != 0:
            info_table.ix[index, 'INFO_w'] = total / tuple_ct * (0.0 - no / total * np.log2(no/total))
        elif yes != 0 and no == 0:
            info_table.ix[index, 'INFO_w'] = total / tuple_ct * (0.0 - yes / total * np.log2(yes/total))
        else:
            info_table.ix[index, 'INFO_w'] = 0.0
        info_table.ix[index, 'GINI_w'] = total / tuple_ct * (1.0 - np.square(yes/total) - np.square(no/total))

        info_table.ix[index, 'SPLIT_w'] = 0.0 - total / tuple_ct * np.log2(total / tuple_ct)


    # print(info_table)

    yes_ct = info_table['YES'].sum()
    no_ct = info_table['NO'].sum()

    # Calculate Information gain:
    info = 0.0 - yes_ct / tuple_ct * np.log2(yes_ct/tuple_ct) - no_ct / tuple_ct * np.log2(no_ct/tuple_ct)
    # print('info', info)
    info_a = info_table['INFO_w'].sum()
    ig = info - info_a

    # Calculate gain ratio:
    split_info_a = info_table['SPLIT_w'].sum()
    gr = ig / split_info_a

    # Calculate gini index:
    gini = 1.0 - np.square(yes_ct/tuple_ct) - np.square(no_ct/tuple_ct)
    gini_a = info_table['GINI_w'].sum()
    gi = gini - gini_a

    return ig, gr, gi

raw = []
count = 0
for line in sys.stdin:
    if count == 0 or count == 1:
        line = line.replace('\n', '')
    else:
        line = line.replace('\n', '').lower()
    raw.append(line)
    count += 1
setsize = int(raw[0])
attr_list = raw[1].split(',')
data = raw[1:]
df = pd.DataFrame([feature.split(',') for feature in data])
df = df.rename(columns=df.iloc[0]).drop(df.index[0])

# print(df)

ig_list = []
gr_list = []
gi_list = []

for i in range(len(df.columns)-1):
    sub_df = df.take([i, len(df.columns)-1], axis=1)
    # print(list(sub_df))
    info_gain, gain_ratio, gini_index = get_scores(sub_df)
    ig_list.append(info_gain)
    gr_list.append(gain_ratio)
    gi_list.append(gini_index)

# print('ig', ig_list)
# print('gr', gr_list)
# print('gi', gi_list)

print(attr_list[ig_list.index(max(ig_list))])
print(attr_list[gr_list.index(max(gr_list))])
print(attr_list[gi_list.index(max(gi_list))])
