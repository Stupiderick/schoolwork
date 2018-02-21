from sklearn.decomposition import PCA
from matplotlib import pyplot as plt

'''
Get the five patient indices which have short distance to patient P.
'''

numOfPCList = [100, 10, 2, 1, -1]
explainedVarCum = []

for numOfPC in numOfPCList:
    # dictionary {index: distance}
    manhattanDistance = {}
    euclideanDistance = {}
    supremumDistance = {}
    cosineDistance = {}
    # get the rank of the indices based on distance
    manhattanRanking = []
    euclideanRanking = []
    supremumRanking = []
    cosineRanking = []
    # take the first five indices
    manhattanFinalOutput = []
    euclideanFinalOutput = []
    supremumFinalOutput = []
    cosineFinalOutput = []

    with open('Q4-analysis-input.in', 'r') as myFile:
        datalist = myFile.read().replace(' \n','\n').splitlines()

        # Line 1-4
        dimensions = int(datalist[0])
        numOfPatients = int(datalist[1])
        typeOfDistance = int(datalist[2])
        #numOfPC = int(datalist[3])

        # if number of PC is -1, it means we do not need to implement PCA
        if numOfPC == -1:
            # Line 5
            patientP = datalist[4].split(' ')
            patientP = [int(i) for i in patientP]

            # After Line 6
            patients = []
            for patient in datalist[5:]:
                patient = patient.split(' ')
                patient = [int(i) for i in patient]
                patients.append(patient)
        # otherwise, we need to consider PCA
        else:
            allPatients = []
            for patient in datalist[4:]:
                patient = patient.split(' ')
                patient = [int(i) for i in patient]
                allPatients.append(patient)

            myPCA = PCA(n_components=numOfPC)
            allPatients = myPCA.fit_transform(allPatients)
            # get the new arrays of patient P and the other patients
            patientP = allPatients[0]
            patients = allPatients[1:]
            # change new dimension as number of components
            dimensions = numOfPC

            if numOfPC == 100:
                explainedVarCum = myPCA.explained_variance_.cumsum()


        # Manhattan distance
        for i in range(numOfPatients):
            result = 0.0
            for j in range(dimensions):
                result += abs(patientP[j] - patients[i][j])
            manhattanDistance[i] = result

        # Euclidean distance
        for i in range(numOfPatients):
            result = 0.0
            for j in range(dimensions):
                result += (patientP[j] - patients[i][j]) ** 2
            euclideanDistance[i] = result ** 0.5

        # Supremum distance
        for i in range(numOfPatients):
            resultList = []
            for j in range(dimensions):
                resultList.append(abs(patientP[j] - patients[i][j]))
            supremumDistance[i] = max(resultList)

        # Cosine distance
        for i in range(numOfPatients):
            dotxy = 0.0
            dotx = 0.0
            doty = 0.0
            for j in range(dimensions):
                dotxy += patientP[j] * patients[i][j]
                dotx += patientP[j] ** 2
                doty += patients[i][j] ** 2
            cosineDistance[i] = 1.0 - dotxy / (dotx * doty) ** 0.5

        # get the same indices
        for item in manhattanDistance:
            manhattanRanking.append(item)
        for item in euclideanDistance:
            euclideanRanking.append(item)
        for item in supremumDistance:
            supremumRanking.append(item)
        for item in cosineDistance:
            cosineRanking.append(item)

        # sorted by values (distance)
        manhattanRanking = sorted(manhattanRanking, key=manhattanDistance.__getitem__)
        euclideanRanking = sorted(euclideanRanking, key=euclideanDistance.__getitem__)
        supremumRanking = sorted(supremumRanking, key=supremumDistance.__getitem__)
        cosineRanking = sorted(cosineRanking, key=cosineDistance.__getitem__)

        # get first five indices
        for i in range(5):
            manhattanFinalOutput.append(manhattanRanking[i] + 1)
        for i in range(5):
            euclideanFinalOutput.append(euclideanRanking[i] + 1)
        for i in range(5):
            supremumFinalOutput.append(supremumRanking[i] + 1)
        for i in range(5):
            cosineFinalOutput.append(cosineRanking[i] + 1)

        print('Manhattan ranking: ', manhattanFinalOutput)
        print('Euclidean ranking: ', euclideanFinalOutput)
        print('Supremum ranking:  ', supremumFinalOutput)
        print('Cosine ranking:    ', cosineFinalOutput)
        if numOfPC == -1:
            print('No PCA')
        else:
            print('under PC: ', numOfPC)

xAxis = list(range(1, 101))
plt.plot(xAxis, explainedVarCum)
plt.title('PC vs Cumulative explained variance')
plt.xlabel('Principal components')
plt.ylabel('Cumulative explained variance')
plt.grid(color='black', linestyle='-', linewidth=0.3)
plt.show()
