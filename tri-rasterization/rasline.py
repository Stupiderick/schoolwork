def rasLine(p1x, p1y, p2x, p2y, plot=True):
    '''
    find out the points that are needed to be plotted
    '''

    coordList = []

    if p1x == p2x:
        if p1y > p2y:
            for i in range(p2y, p1y+1):
                coordList.append((i, p1x))
            if plot:
                plotLine(coordList, p1x-1, p2x+1, p2y, p1y)
        else:
            for i in range(p1y, p2y+1):
                coordList.append((i, p1x))
            if plot:
                plotLine(coordList, p1x-1, p2x+1, p1y, p2y)

        return coordList


    if p1y == p2y:
        if p1x > p2x:
            for i in range(p2x, p1x+1):
                coordList.append((i, p1y))
            if plot:
                plotLine(coordList, p2x, p1x, p1y-1, p2y+1)
        else:
            for i in range(p1x, p2x+1):
                coordList.append((i, p1y))
            if plot:
                plotLine(coordList, p1x, p2x, p1y-1, p2y+1)

        return coordList

    # If p1x is less than p2x, we switch them to ensure point 1 is on the left
    #  of point 2.
    if p1x > p2x:
        p1x, p1y, p2x, p2y = switchPoint(p1x, p1y, p2x, p2y)

    slope = float((p2y - p1y) / (p2x - p1x))
    yIntercept = p1y - slope * p1x

    if slope > 0:
        tend = 1
    else:
        tend = -1

    pointX = p1x
    pointY = p1y

    while pointX != p2x and pointY != p2y:
        coordList.append((pointX, pointY))

        yNext = slope * (pointX + 1) + yIntercept
        if yNext - pointY > 1:
            for i in range(pointY+1, int(yNext)):
                coordList.append((pointX, i))
            pointY = int(yNext)

        pointX += 1
        if yNext - pointY > pointY + 1 - yNext:
            pointY += 1

    coordList.append((p2x, p2y))

    if plot:
        if p2y > p1y:
            plotLine(coordList, p1x, p2x, p1y, p2y)
        else:
            plotLine(coordList, p1x, p2x, p2y, p1y)

    return coordList


def plotLine(coordList, minX, maxX, minY, maxY):
    for i in range(minX, maxX+1):
        for j in range(maxY+1, minY, -1):
            if (i, j) in coordList:
                print('*', end='')
            else:
                print(' ', end='')
        print()



def switchPoint(a, b, c, d):
    t1 = a
    t2 = b
    a = c
    b = d
    c = t1
    d = t2
    return a, b, c, d
