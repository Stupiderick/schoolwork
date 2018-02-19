
with open('dataset-sample.txt', 'r') as myFile:
    datalist = myFile.read().splitlines()

    dataDimensions = int(datalist[0])
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
