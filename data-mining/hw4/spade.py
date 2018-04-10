import sys
import re
import itertools

'''
item_table = [[SID(0), EID(1), Items(2)], ...]

len_one_table = {base(e.g.: 'a', 'b'): base_table, ...}
base_table = [[SID(0), EID(1)], ...]

'''

item_table = []
big_table = []
base_list = []
big_len_table = []
biggest_len_table = {}

stopwordslist = 'a an are as at by be for from has he in is it its of on that the to was were will with'.split()

senlist = []
for line in sys.stdin:
    line = line.replace(' \n', '\n').replace('\n', '').replace(' and ', ',').replace(', ', ',').replace('.', '').lower().split()
    senlist.append(line)

minsup = int(senlist[0][0])
senlist = senlist[1:]

# remove stopwords from the list
for stopword in stopwordslist:
    for line in senlist:
        try:
            line.remove(stopword)
        except ValueError:
            pass

# group into sets
for i in range(len(senlist)):
    for j in range(len(senlist[i])):
        senlist[i][j] = set(sorted(senlist[i][j].split(',')))
        item_table.append([i+1, j+1, senlist[i][j]])
        for base in senlist[i][j]:
            if base not in base_list:
                base_list.append(base)

base_list = sorted(base_list)

# get the length of the longest sequence.
lenlist = []
for person in senlist:
    len_count = 0
    for trans in person:
        for candi in trans:
            len_count += 1
    lenlist.append(len_count)
len_max = max(lenlist)

# build length-1 table
len_one_table = {}
len_one_sup = {}
len_one_list = []
for base in base_list:
    countlist = []
    base_table = []
    for item in item_table:
        if base in item[2]:
            base_table.append([item[0], item[1]])
            if item[0] not in countlist:
                countlist.append(item[0])
    if len(countlist) >= minsup:
        len_one_table.update({base: base_table})
        len_one_sup.update({base: len(countlist)})
        len_one_list.append(base)

update_one_sup = {}
for key, value in len_one_sup.items():
    if type(key) is not tuple:
        update_one_sup[(key,)] = value
    else:
        update_one_sup[key] = value

big_table.append(len_one_table)
big_len_table.append(update_one_sup)
biggest_len_table = update_one_sup

# build length-2 table
len_two_table = {}
len_two_sup = {}
c2 = list([p for p in itertools.product(len_one_list, repeat=2)])
c2s = list(itertools.combinations(len_one_list, 2))

# sequence in different transactions
for candidate in c2:
    count = 0
    candi_table = []
    first_table = len_one_table[candidate[0]]
    second_table = len_one_table[candidate[1]]

    for item1 in first_table:
        for item2 in second_table:
            if item1[0] == item2[0]:
                # e.g.: candidate=('a', 'b'), do iff 'a' ahead of 'b' <ab>.
                if candidate[0] < candidate[1] and item1[1] < item2[1]:
                    candi_table.append([item1[0], item1[1], item2[1]])
                    # break
                # e.g.: candidate=('b', 'a'), do iff 'b' ahead of 'a' <ba>.
                if candidate[0] >= candidate[1] and item1[1] < item2[1]:
                    candi_table.append([item1[0], item1[1], item2[1]])
                    # break

    # count number of diff SID
    candi_list = []
    count = 0
    for candi in candi_table:
        if candi[0] not in candi_list:
            count += 1
            candi_list.append(candi[0])

    if count >= minsup:
        len_two_table.update({candidate: candi_table})
        len_two_sup.update({candidate: count})

# sequence in the same transaction
# NOTE: items in the same transaction denoted as tuples
for candidate in c2s:
    count = 0
    candi_table = []
    first_table = len_one_table[candidate[0]]
    second_table = len_one_table[candidate[1]]

    for item1 in first_table:
        for item2 in second_table:
            if item1[0] == item2[0]:
                # e.g.: candidate=('a', 'b'), do iff 'a' with 'b' (ab).
                if candidate[0] < candidate[1] and item1[1] == item2[1]:
                    candi_table.append([item1[0], item1[1], item2[1]])
                    # break
    # count number of diff SID
    candi_list = []
    count = 0
    for candi in candi_table:
        if candi[0] not in candi_list:
            count += 1
            candi_list.append(candi[0])

    if count >= minsup:
        nc = []
        for i in range(len(candidate)):
            nc.append(tuple(candidate[i]))

        candidate = tuple(nc)
        len_two_table.update({candidate: candi_table})
        len_two_sup.update({candidate: count})

if len_two_table != {}:
    big_table.append(len_two_table)
    big_len_table.append(len_two_sup)
    biggest_len_table = len_two_sup


# finding pattern with length larger than one.
if len_max <= 2:
    len_max = 2
for len_pat in range(3, len_max+1):
    len_x_table = {}
    len_x_sup = {}
    # len_match: length of section of patterns should match with each other when concatenating.
    len_match = len_pat - 2
    for candidate1, table1 in big_table[len_pat-2].items():
        for candidate2, table2 in big_table[len_pat-2].items():
            # check if the two patterns can be connected
            match = False
            share1 = candidate1[0-len_match:]
            share2 = candidate2[:len_match]
            cute_flag = False

            if type(share1[0]) is tuple and type(share2[0]) is not tuple:
                if [share1[0][0]] + list(share1[1:]) == list(share2):
                    match = True
            elif type(share1[0]) is not tuple and type(share2[0]) is tuple:
                if list(share1) == [share2[0][0]] + list(share2[1:]):
                    match = True
                    # candidate2 has tuple, but candidate1 not
                    cute_flag = True
            else:
                if share1 == share2:
                    match = True

            if match == True:
                candi_table = []
                for item1 in table1:
                    for item2 in table2:
                        # when SIDs match and shared EIDs match as well
                        if item1[0-len_match:] == item2[1:len_match+1] and item1[0] == item2[0]:
                            candi_table.append(item1[:] + item2[-1:])
                            # break
                if cute_flag == True:
                    new_candi = candidate1[:1] + candidate2
                else:
                    new_candi = candidate1 + candidate2[-1:]

                # count number of diff SID
                candi_list = []
                count = 0
                for candi in candi_table:
                    if candi[0] not in candi_list:
                        count += 1
                        candi_list.append(candi[0])
                # check minsup
                if count >= minsup:
                    len_x_table.update({new_candi: candi_table})
                    len_x_sup.update({new_candi: count})

    if len_x_table == {}:
        break
    big_table.append(len_x_table)
    big_len_table.append(len_x_sup)
    biggest_len_table = len_x_sup

# get sorted index list, but with tuples.
sort_list = []
for key, value in biggest_len_table.items():
    sort_list.append(key)

# since tuples will affect sorting, we need to break down the tuple
sl_no_tuple = []
for i in sort_list:
    temp = []
    for j in i:
        if type(j) is tuple:
            for k in j:
                temp.append(k)
        else:
            temp.append(j)
    sl_no_tuple.append(temp)

index_list = sorted(range(len(sl_no_tuple)), key=lambda k: sl_no_tuple[k])
new_sort_list = []
for i in index_list:
    new_sort_list.append(sort_list[i])

sort_list = new_sort_list

# print('bt', big_table)
# print('blt', big_len_table)
# print('bglt', biggest_len_table)
# print('sl', sort_list)

freq_list = []
for candi in sort_list:
    if biggest_len_table[candi] not in freq_list:
        freq_list.append(biggest_len_table[candi])
freq_list = sorted(freq_list, reverse=True)
# print('fl', freq_list)

update_sort_list = []
for number in freq_list:
    for candi in sort_list:
        if biggest_len_table[candi] == number:
            update_sort_list.append(candi)

sort_list = update_sort_list


# output
one_string = ''
for candi in sort_list:
    one_string += str(biggest_len_table[candi]) + ' ['
    wickedflag = False
    for i in range(len(candi)):
        if type(candi[i]) is tuple and wickedflag == False:
            wickedflag = True
            one_string += '(' + candi[i][0]
        elif type(candi[i]) is tuple and wickedflag == True:
            one_string += candi[i][0]
        elif type(candi[i]) is not tuple and wickedflag == True:
            one_string = one_string[:-1]
            one_string += ') ' + candi[i]
            wickedflag = False
        else:
            one_string += candi[i]

        if i != len(candi) - 1:
            one_string += ' '
        else:
            if wickedflag == False:
                one_string += ']\n'
            else:
                one_string += ')]\n'

one_string = one_string[:-1]
print(one_string)
