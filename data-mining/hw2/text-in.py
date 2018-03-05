import sys
import itertools as its

myWrite = sys.stdout.write

myFile = open('input.txt', 'r')
biglist = [] # the well-splited list
charlist = []
bigcharlist = []

inputlines = myFile.read().splitlines()

partition = int(inputlines[0])
inputlines = inputlines[1:]

for i in range(len(inputlines)):
    inputlines[i] = inputlines[i].split()

dimension = len(inputlines[1])
size = int(dimension / partition) # how many dimensions per partition

for i in range(partition):
    sublist = []
    for j in inputlines:
        sublist.append(j[i*size: (i+1)*size])

    biglist.append(sublist)


'''consider the partitions'''
for part in biglist:
    chardict = {}
    '''different lengths for combination'''
    for combi in range(len(part[0])):
        # get the list for one item to all items
        #  e.g. [(1.),(2.)...]
        iterlist = list(its.combinations(range(size), combi+1))
        '''get a combination'''
        for positions in iterlist:
            nums = []
            datacol = []

            for p in positions:
                nums.append(p)

            for row in part:
                subdatacol = []
                for num in nums:
                    subdatacol.append(row[num])
                datacol.append(subdatacol)
            datacol = sorted(datacol)

            for slist in datacol:
                chara = ''
                for stuff in slist:
                    chara = chara + stuff + ' '
                if chara in charlist:
                    chardict[chara] += 1
                else:
                    charlist.append(chara)
                    chardict[chara] = 1
                    # print(chara)
    bigcharlist.append(chardict)

print(bigcharlist)

for i in range(len(bigcharlist)):
    for key, value in bigcharlist[i].items():
        myWrite(key + ': ' + str(value) + '\n')
    if i != len(bigcharlist) - 1:
        myWrite('\n')

myFile.close()
