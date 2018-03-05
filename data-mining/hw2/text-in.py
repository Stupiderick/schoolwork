import pandas as pd
import numpy as np
import sys
import itertools as its

myWrite = sys.stdout.write

myFile = open('input.txt', 'r')
biglist = [] # the well-splited list
chardict = {}
charlist = []

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

# print(biglist)

for part in biglist:
    for combi in range(len(part[0])):
        # get the list for one item to all items
        #  e.g. [(1.),(2.)...]
        iterlist = list(its.combinations(range(size), combi+1))

        for positions in iterlist:
            nums = []

            for p in positions:
                nums.append(p)
            # print(nums)

            for row in part:
                chara = ''
                for num in nums:
                    chara = chara + row[num] + ' '
                if chara in charlist:
                    chardict[chara] += 1
                else:
                    charlist.append(chara)
                    chardict[chara] = 1
                    # print(chara)

print(chardict)
myFile.close()
