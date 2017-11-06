def rasLine(p1x, p1y, p2x, p2y):
    if p1x == p2x:
        return

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
    coordList = []

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


    return coordList


def switchPoint(a, b, c, d):
    t1 = a
    t2 = b

    a = c
    b = d

    c = t1
    d = t2

    return a, b, c, d
