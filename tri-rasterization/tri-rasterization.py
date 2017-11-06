def rasLine(p1x, p1y, p2x, p2y):
    if p1x < p2x:
        p1x, p1y, p2x, p2y = switchPoint(p1x, p1y, p2x, p2y)
    


def switchPoint(a, b, c, d):
    t1 = a
    t2 = b

    a = c
    b = d

    c = t1
    d = t2

    return a, b, c, d
