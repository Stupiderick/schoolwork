import sys

'''
Get the five patient indices which have short distance to patient P.
'''
write = sys.stdout.write
# dictionary {index: distance}
distance = {}
# get the rank of the indices based on distance
ranking = []
# take the first five indices
final_output = []

datalist = []

for line in sys.stdin:
    line = line.replace(' \n', '') # in case there is an error by putting a space in the end
    line = line.replace('\n', '')
    datalist.append(line)

# Line 1-4
dimensions = int(datalist[0])
numOfPatients = int(datalist[1])
typeOfDistance = int(datalist[2])
numOfPC = int(datalist[3])


# Line 5
patientP = datalist[4].split(' ')
patientP = [int(i) for i in patientP]

# After Line 6
patients = []
for patientData in datalist[5:]:
    patient = patientData.split(' ')
    patient = [int(i) for i in patient]
    patients.append(patient)


# Manhattan distance
if typeOfDistance == 1:
    for i in range(numOfPatients):
        result = 0.0
        for j in range(dimensions):
            result += abs(patientP[j] - patients[i][j])
        distance[i] = result

# Euclidean distance
elif typeOfDistance == 2:
    for i in range(numOfPatients):
        result = 0.0
        for j in range(dimensions):
            result += (patientP[j] - patients[i][j]) ** 2
        distance[i] = result ** 0.5

# Supremum distance
elif typeOfDistance == 3:
    for i in range(numOfPatients):
        resultList = []
        for j in range(dimensions):
            resultList.append(abs(patientP[j] - patients[i][j]))
        distance[i] = max(resultList)

# Cosine distance
elif typeOfDistance == 4:
    for i in range(numOfPatients):
        dotxy = 0.0
        dotx = 0.0
        doty = 0.0
        for j in range(dimensions):
            dotxy += patientP[j] * patients[i][j]
            dotx += patientP[j] ** 2
            doty += patients[i][j] ** 2
        dotx = dotx ** 0.5
        doty = doty ** 0.5
        distance[i] = 1.0 - dotxy / (dotx * doty)

else:
    print('Distance code is wrong!')
    exit()

# get the same indices
for item in distance:
    ranking.append(item)

# sorted by values (distance)
ranking = sorted(ranking, key=distance.__getitem__)

# get first five indices
for i in range(5):
    final_output.append(ranking[i] + 1)

# write to stdout
write(str(final_output[0]) + '\n')
write(str(final_output[1]) + '\n')
write(str(final_output[2]) + '\n')
write(str(final_output[3]) + '\n')
write(str(final_output[4]) + '\n')
