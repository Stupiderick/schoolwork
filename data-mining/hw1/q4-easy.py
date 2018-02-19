

distance = {}
ranking = []
final_output = []

with open('dataset-sample2.txt', 'r') as myFile:
    datalist = myFile.read().splitlines()

    dimensions = int(datalist[0])
    numOfPatients = int(datalist[1])
    typeOfDistance = int(datalist[2])
    numOfPC = int(datalist[3])

    patientP = datalist[4].split(' ')
    patientP = [int(i) for i in patientP]

    patients = []
    for patient in datalist[5:]:
        patient = patient.split(' ')
        patient = [int(i) for i in patient]
        patients.append(patient)

    if typeOfDistance == 1:
        for i in range(numOfPatients):
            result = 0.0
            for j in range(dimensions):
                result += abs(patientP[j] - patients[i][j])
            distance[i] = result

    elif typeOfDistance == 2:
        for i in range(numOfPatients):
            result = 0.0
            for j in range(dimensions):
                result += (patientP[j] - patients[i][j]) ** 2
            distance[i] = result ** 0.5

    elif typeOfDistance == 3:
        for i in range(numOfPatients):
            resultList = []
            for j in range(dimensions):
                resultList.append(abs(patientP[j] - patients[i][j]))
            distance[i] = max(resultList)

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

for item in distance:
    ranking.append(item)
ranking = sorted(ranking, key=distance.__getitem__)
for i in range(5):
    final_output.append(ranking[i] + 1)
print(final_output)
