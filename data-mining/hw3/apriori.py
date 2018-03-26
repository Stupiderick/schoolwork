import sys
import itertools

dict_str = {}
dict_list = {}
greaterdict = {}
closedict = {}

inputlines = []
table = []
seed = {}
mlength = 0

for line in sys.stdin:
    line = line.replace('\n', '')
    inputlines.append(line)

minsup = int(inputlines[0])

for i in range(1, len(inputlines)):
    transaction = inputlines[i].split()
    table.append(transaction)

for transaction in table:
    if len(transaction) > mlength:
        mlength = len(transaction)
    for item in transaction:
        if item in seed:
            seed[item] += 1
        else:
            seed[item] = 1

# remove items sup < minsup
badkeys = []
goodkeys = []
for key, value in seed.items():
    if value < minsup:
        badkeys.append(key)
    else:
        goodkeys.append(key)

for key in badkeys:
    del seed[key]

dict_str.update(seed)
goodkeys = sorted(goodkeys)
'''-------------------------------------------------'''

for sublength in range(2, mlength):
    glist = list(itertools.combinations(goodkeys, sublength))
    gdict = {}

    for i in glist:
        for j in table:
            if set(i) <= set(j):
                if i in gdict:
                    gdict[i] += 1
                else:
                    gdict[i] = 1

    ibadkeys = []
    igoodkeys = []
    for key, value in gdict.items():
        if value < minsup:
            ibadkeys.append(key)
        else:
            igoodkeys.append(key)

    for key in ibadkeys:
        del gdict[key]

    if gdict == {}:
        break

    ndict = {}
    for key in gdict:
        st = ' '.join(str(x) for x in key)
        ndict[st] = gdict[key]

    dict_str.update(ndict)

'''-----------------------------------------------'''
suplist = []
for key, value in dict_str.items():
    if value not in suplist:
        suplist.append(value)
        greaterdict[value] = []
    greaterdict[value].append(key)

for key in greaterdict:
    greaterdict[key] = sorted(greaterdict[key])

suplist = sorted(suplist, reverse=True)

output1 = ''
for i in suplist:
    for j in greaterdict[i]:
        oneline = str(i) + ' [' + j + ']\n'
        output1 += oneline

print(output1)

for key, value in greaterdict.items():
    dict_list[key] = []
    for item in value:
        dict_list[key].append(item.split())

for key, value in dict_list.items():
    closedict[key] = []
    for i in value:
        flag = True
        for j in value:
            if set(i) < set(j):
                flag = False
        if flag:
            closedict[key].append(i)

output2 = ''
for i in suplist:
    if closedict[i] != []:
        for j in closedict[i]:
            k = ' '.join(str(x) for x in j)
            oneline = str(i) + ' [' + k + ']\n'
            output2 += oneline

output2 = output2[:-1]
print(output2)
