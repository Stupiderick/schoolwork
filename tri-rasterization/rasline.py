def rasLine(p1x, p1y, p2x, p2y, plot=True, pointStyle='*'):
    '''
    find out the points that are needed to be plotted
    '''

    coordList = []

    # Case: vertical line
    if p1x == p2x:
        if p1y > p2y:
            for i in range(p2y, p1y+1):
                coordList.append((i, p1x))
            if plot:
                plotLine(coordList, p1x-1, p2x+1, p2y, p1y, pointStyle)
        else:
            for i in range(p1y, p2y+1):
                coordList.append((i, p1x))
            if plot:
                plotLine(coordList, p1x-1, p2x+1, p1y, p2y, pointStyle)

        return coordList

    # Case: horizontal line
    if p1y == p2y:
        if p1x > p2x:
            for i in range(p2x, p1x+1):
                coordList.append((i, p1y))
            if plot:
                plotLine(coordList, p2x, p1x, p1y-1, p2y+1, pointStyle)
        else:
            for i in range(p1x, p2x+1):
                coordList.append((i, p1y))
            if plot:
                plotLine(coordList, p1x, p2x, p1y-1, p2y+1, pointStyle)

        return coordList

    # If p1x is less than p2x, we switch them to ensure point 1 is on the left
    #  of point 2.
    if p1x > p2x:
        p1x, p1y, p2x, p2y = switchPoint(p1x, p1y, p2x, p2y)

    slope = float((p2y - p1y) / (p2x - p1x))
    yIntercept = p1y - slope * p1x
    print(slope)
    pointX = p1x
    pointY = p1y

    if slope <= 1 and slope > 0:
        while pointX != p2x or pointY != p2y:
            coordList.append((pointX, pointY))

            yNext = slope * (pointX + 1) + yIntercept

            pointX += 1
            #print(pointX, pointY)

            if yNext - pointY > pointY + 1 - yNext:
                pointY += 1
    elif slope > 1 :
        while pointX != p2x or pointY != p2y:
            coordList.append((pointX, pointY))

            xNext = ((pointY + 1) - yIntercept) / slope

            pointY += 1
            #print(pointX, pointY)

            if xNext - pointX > pointX + 1 - xNext:
                pointX += 1

    coordList.append((p2x, p2y))

    if plot:
        if p2y > p1y:
            plotLine(coordList, p1x, p2x, p1y, p2y, pointStyle)
        else:
            plotLine(coordList, p1x, p2x, p2y, p1y, pointStyle)

    return coordList


def plotLine(coordList, minX, maxX, minY, maxY, pointStyle):
    for j in range(maxY, minY-1, -1):
        print("{0:02d}".format(j), end='')
        for i in range(minX, maxX+1):
            if (i, j) in coordList:
                print(pointStyle, end=' ')
            else:
                print('  ', end='')
        # print a newline.
        print()



def switchPoint(a, b, c, d):
    t1 = a
    t2 = b
    a = c
    b = d
    c = t1
    d = t2
    return a, b, c, d
