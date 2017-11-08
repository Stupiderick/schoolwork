def rasLine(point1, point2, plot=True, pointStyle='*', grid=False):
    '''
    find out the points that are needed to be plotted

    Parameters
    ----------
    point1: tuple of the coord of point 1
    point2: tuple of the coord of point 2
    plot=True: whether plot the diagram
    pointStyle='*': which char to be used to represent points
    grid=False: whether set the grid in the graph

    Return
    ------
    list of coords of point
    '''

    if plot == False:
        grid = False

    coordList = []
    p1x = point1[0]
    p1y = point1[1]
    p2x = point2[0]
    p2y = point2[1]


    # Case: vertical line
    if p1x == p2x:
        if p1y > p2y:
            for i in range(p2y, p1y+1):
                coordList.append((p1x, i))
            if plot:
                plotLine(coordList, p1x-1, p2x+1, p2y, p1y, pointStyle, grid)
        else:
            for i in range(p1y, p2y+1):
                coordList.append((p1x, i))
            if plot:
                plotLine(coordList, p1x-1, p2x+1, p1y, p2y, pointStyle, grid)

        return coordList

    # Case: horizontal line
    if p1y == p2y:
        if p1x > p2x:
            for i in range(p2x, p1x+1):
                coordList.append((i, p1y))
            if plot:
                plotLine(coordList, p2x, p1x, p1y-1, p2y+1, pointStyle, grid)
        else:
            for i in range(p1x, p2x+1):
                coordList.append((i, p1y))
            if plot:
                plotLine(coordList, p1x, p2x, p1y-1, p2y+1, pointStyle, grid)

        return coordList

    # If p1x is less than p2x, we switch them to ensure point 1 is on the left
    #  of point 2.
    if p1x > p2x:
        p1x, p1y, p2x, p2y = switchPoint(p1x, p1y, p2x, p2y)

    slope = float((p2y - p1y) / (p2x - p1x))
    yIntercept = p1y - slope * p1x
    pointX = p1x
    pointY = p1y

    if slope <= 1 and slope > 0:
        while pointX != p2x or pointY != p2y:
            coordList.append((pointX, pointY))
            yNext = slope * (pointX + 1) + yIntercept
            pointX += 1

            if yNext - pointY > pointY + 1 - yNext:
                pointY += 1
    elif slope > 1 :
        while pointX != p2x or pointY != p2y:
            coordList.append((pointX, pointY))
            xNext = ((pointY + 1) - yIntercept) / slope
            pointY += 1

            if xNext - pointX > pointX + 1 - xNext:
                pointX += 1
    elif slope < 0 and slope > -1:
        while pointX != p2x or pointY != p2y:
            coordList.append((pointX, pointY))
            yNext = slope * (pointX + 1) + yIntercept
            pointX += 1

            if yNext - pointY < pointY - 1 - yNext:
                pointY -= 1
    else:
        while pointX != p2x or pointY != p2y:
            coordList.append((pointX, pointY))
            xNext = ((pointY - 1) - yIntercept) / slope
            pointY -= 1

            if xNext - pointX > pointX + 1 - xNext:
                pointX += 1

    coordList.append((p2x, p2y))

    if plot:
        if p2y > p1y:
            plotLine(coordList, p1x, p2x, p1y, p2y, pointStyle, grid)
        else:
            plotLine(coordList, p1x, p2x, p2y, p1y, pointStyle, grid)

    return coordList


def plotLine(coordList, minX, maxX, minY, maxY, pointStyle, grid):
    for j in range(maxY, minY-1, -1):
        # Case with grid
        if grid:
            print("{0:02d} ".format(j), end='')
            for i in range(minX, maxX+1):
                if (i, j) in coordList:
                    print('|' + pointStyle, end='')
                else:
                    print('| ', end='')
            print('|')
            for i in range(minX, maxX+3):
                print('--', end='')
        # case without grid
        else:
            for i in range(minX, maxX+1):
                if (i, j) in coordList:
                    print(pointStyle, end=' ')
                else:
                    print('  ', end='')
        # print a newline.
        print()

    if grid:
        print('y  |', end='')
        for i in range(minX, maxX+1):
            print(str(int(i / 10)) + '|', end='')
        print()
        print(' x |', end='')
        for i in range(minX, maxX+1):
            print(str(i % 10) + '|', end='')
        print()



def switchPoint(a, b, c, d):
    t1 = a
    t2 = b
    a = c
    b = d
    c = t1
    d = t2
    return a, b, c, d
