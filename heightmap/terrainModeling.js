
/**
 * limit the highest point.
 * @param {number} n
 * @param {array} vertexArray
 * @param {number} max
 * @param {number} min
 */
function normalizeHeight(n, vertexArray, max, min) {
    for (var i = 0; i < (n + 1) ** 2; i++) {
        if (vertexArray[getHeightIndex(i)] > max) {
            vertexArray[getHeightIndex(i)] = max;
        } else if (vertexArray[getHeightIndex(i)] < min) {
            vertexArray[getHeightIndex(i)] = min;
        }
    }
}

/**
 * generate color based on height.
 * @param {number} n
 * @param {array} colorArray
 * @param {array} vertexArray
 */
function generateColor(n, colorArray, vertexArray) {
    for (var i = 0; i < (n + 1) ** 2; i++) {
        if (vertexArray[getHeightIndex(i)] > 0.15) {
            colorArray.push(1, 1, 1);  // white
        } else if (vertexArray[getHeightIndex(i)] > 0.05) {
            colorArray.push(0.27, 0.24, 0.22);  // brown
        } else if (vertexArray[getHeightIndex(i)] > -0.1) {
            colorArray.push(0.12, 0.58, 0.18);  // leave
        } else {
            colorArray.push(0.0, 0.0, 1.0);  // grass
        }
    }
}

/**
 * Get the height of the mid point.
 * @param {number} n
 * @param {number} p1 position of the top-left vertex.
 * @param {number} p2 position of the top-right vertex.
 * @param {number} p3 position of the bottom-left vertex.
 * @param {number} p4 position of the bottom-right vertex.
 * @param {Array} vertexArray Array that will contain vertices generated
 * @param {Array} heightCheck check whether the height of vertex has been comfirmed
 * @param {number} lv of the check.
 */
function getMidPointHeight(n, p1, p2, p3, p4, vertexArray, heightCheck, lv) {
    var midPoint = (p1 + p2 + p3 + p4) / 4;  // Get the position of the mid point.
    var heightP1 = vertexArray[getHeightIndex(p1)];
    var heightP2 = vertexArray[getHeightIndex(p2)];
    var heightP3 = vertexArray[getHeightIndex(p3)];
    var heightP4 = vertexArray[getHeightIndex(p4)];
    var avgHeight = (heightP1 + heightP2 + heightP3 + heightP4) / 4;

    vertexArray[getHeightIndex(midPoint)] = avgHeight + (Math.random() * 1.05 - 0.5) / (lv * 2);
    heightCheck[midPoint] = true; // have checked the height of the midpoint.
}

/**
 * Get the height of the neighbor vertices.
 * @param {number} n
 * @param {number} p1 position of the top-left vertex.
 * @param {number} p2 position of the top-right vertex.
 * @param {number} p3 position of the bottom-left vertex.
 * @param {number} p4 position of the bottom-right vertex.
 * @param {Array} vertexArray Array that will contain vertices generated
 * @param {Array} heightCheck check whether the height of vertex has been comfirmed
 * @param {number} lv of the check.
 */
function getNeighborsHeight(n, p1, p2, p3, p4, vertexArray, heightCheck, lv) {
    var top = (p1 + p2) / 2;
    var bottom = (p3 + p4) / 2;
    var left = (p3 + p1) / 2;
    var right = (p4 + p2) / 2;

    getHeightFromNeighbors(top, n, vertexArray, heightCheck, lv);
    getHeightFromNeighbors(bottom, n, vertexArray, heightCheck, lv);
    getHeightFromNeighbors(left, n, vertexArray, heightCheck, lv);
    getHeightFromNeighbors(right, n, vertexArray, heightCheck, lv);
}

/**
 * Get the closest top neighbor with height.
 * @param {number} p position of the given vertex.
 * @param {number} n gridN
 * @param {Array} vertexArray Array that will contain vertices generated
 * @param {Array} heightCheck
 * @param {number} lv of the check.
 */
function getHeightFromNeighbors(p, n, vertexArray, heightCheck, lv) {
    var topNeighbor = getTopNeighbor(p, n, heightCheck);
    var bottomNeighbor = getBottomNeighbor(p, n, heightCheck);
    var leftNeighbor = getLeftNeighbor(p, n, heightCheck);
    var rightNeighbor = getRightNeighbor(p, n, heightCheck);

    var numOfHeight = 0;
    var totalHeight = vertexArray[getHeightIndex(topNeighbor)]
                    + vertexArray[getHeightIndex(bottomNeighbor)]
                    + vertexArray[getHeightIndex(leftNeighbor)]
                    + vertexArray[getHeightIndex(rightNeighbor)];
    if (topNeighbor != null) numOfHeight += 1;
    if (bottomNeighbor != null) numOfHeight += 1;
    if (leftNeighbor != null) numOfHeight += 1;
    if (rightNeighbor != null) numOfHeight += 1;

    vertexArray[getHeightIndex(p)] = totalHeight / numOfHeight + (Math.random() * 1.05 - 0.5) / (lv * 2);
    heightCheck[p] = true; // have checked the height of p (one of the neighbors)
}

/**
 * Get the closest top neighbor with height.
 * @param {number} p position of the given vertex.
 * @param {number} n gridN
 * @param {Array} heightCheck
 * @return position of the top neighbor or no neighbor.
 */
function getBottomNeighbor(p, n, heightCheck) {
    // p is at the bottom row.
    if (p <= n) {
        return null;
    }

    while (!heightCheck[p]) {
        p -= (n + 1);
    }
    return p;
}

/**
 * Get the closest bottom neighbor with height.
 * @param {number} p position of the given vertex.
 * @param {number} n gridN
 * @param {Array} heightCheck
 * @return position of the bottom neighbor or no neighbor.
 */
function getTopNeighbor(p, n, heightCheck) {
    // p is at bottom row.
    var len = (n + 1) ** 2;
    if (p >= len - n - 1) {
        return null;
    }

    while (!heightCheck[p]) {
        p += (n + 1);
    }
    return p;
}

/**
 * Get the closest left neighbor with height.
 * @param {number} p position of the given vertex.
 * @param {number} n gridN
 * @param {Array} heightCheck
 * @return position of the left neighbor or no neighbor.
 */
function getLeftNeighbor(p, n, heightCheck) {
    // p is at leftmost column.
    if (p % (n + 1) == 0) {
        return null;
    }

    while (!heightCheck[p]) {
        p -= 1;
    }
    return p;
}

/**
 * Get the closest right neighbor with height.
 * @param {number} p position of the given vertex.
 * @param {number} n gridN
 * @param {Array} heightCheck
 * @return position of the left neighbor or no neighbor.
 */
function getRightNeighbor(p, n, heightCheck) {
    // p is at rightmost column.
    if ((p + 1) % (n + 1) == 0) {
        return null;
    }

    while (!heightCheck[p]) {
        p += 1;
    }
    return p;
}


/**
 * Get the height of the vertex given the position.
 * @param {number} position of the given vertex
 * @return index for X
 */
function getHeightIndex(position) {
    return position * 3 + 2;
}

/**
 * Update the normal array based on the updated heights.
 * @param {number} n
 * @param {array} vertexArray
 * @param {array} normalArray
 */
function updateNormalArray(n, vertexArray, normalArray) {
    var meshNormalArrayX = [];
    var meshNormalArrayY = [];
    var meshNormalArrayZ = [];
    var countLeft = 0;

    for (var i = 0; i < n * n * 2; i++) {
        meshNormalArrayX.push(0);
        meshNormalArrayY.push(0);
        meshNormalArrayZ.push(0);
    }

    // get the normals for left triangles.
    for (var i = 0; i < n; i++) {
        for (var j = 0; j < n; j++) {
            var point1 = i * (n + 1) + j;
            var p1 = [];
            p1.push(vertexArray[getIndexX(point1)]);
            p1.push(vertexArray[getIndexY(point1)]);
            p1.push(vertexArray[getIndexZ(point1)]);

            var point2 = i * (n + 1) + j + 1;
            var p2 = [];
            p2.push(vertexArray[getIndexX(point2)]);
            p2.push(vertexArray[getIndexY(point2)]);
            p2.push(vertexArray[getIndexZ(point2)]);

            var point3 = i * (n + 1) + j + n + 1;
            var p3 = [];
            p3.push(vertexArray[getIndexX(point3)]);
            p3.push(vertexArray[getIndexY(point3)]);
            p3.push(vertexArray[getIndexZ(point3)]);

            l1 = [];
            for (var k = 0; k < 3; k++) {
                l1.push(p2[k] - p1[k]);
            }

            l2 = [];
            for (var k = 0; k < 3; k++) {
                l2.push(p3[k] - p1[k]);
            }
            //console.log(l1);
            //console.log(l2);

            var meshNormal = crossProduct(l1, l2);
            meshNormal = normalization(meshNormal);
            meshNormalArrayX[countLeft] = meshNormal[0];
            meshNormalArrayY[countLeft] = meshNormal[1];
            meshNormalArrayZ[countLeft] = meshNormal[2];

            countLeft += 2;
        }
    }

    //get the normals for right triangles.
    var countRight = 1;
    for (var i = 0; i < n; i++) {
        for (var j = 0; j < n; j++) {
            var point1 = i * (n + 1) + j + 1;
            var p1 = [];
            p1.push(vertexArray[getIndexX(point1)]);
            p1.push(vertexArray[getIndexY(point1)]);
            p1.push(vertexArray[getIndexZ(point1)]);

            var point2 = i * (n + 1) + j + n + 1;
            var p2 = [];
            p2.push(vertexArray[getIndexX(point2)]);
            p2.push(vertexArray[getIndexY(point2)]);
            p2.push(vertexArray[getIndexZ(point2)]);

            var point3 = i * (n + 1) + j + n + 2;
            var p3 = [];
            p3.push(vertexArray[getIndexX(point3)]);
            p3.push(vertexArray[getIndexY(point3)]);
            p3.push(vertexArray[getIndexZ(point3)]);

            var l1 = [];
            for (var k = 0; k < 3; k++) {
                l1.push(p2[k] - p1[k]);
            }

            var l2 = [];
            for (var k = 0; k < 3; k++) {
                l2.push(p3[k] - p1[k]);
            }
            //console.log(l1);
            //console.log(l2);
            var meshNormal = crossProduct(l2, l1);
            //console.log(meshNormal);
            meshNormal = normalization(meshNormal);

            meshNormalArrayX[countRight] = meshNormal[0];
            meshNormalArrayY[countRight] = meshNormal[1];
            meshNormalArrayZ[countRight] = meshNormal[2];

            // console.log(meshNormalArrayX);
            // console.log(meshNormalArrayY);
            // console.log(meshNormalArrayZ);

            countRight += 2;
        }
    }

    // console.log(meshNormalArrayX);
    // console.log(meshNormalArrayY);
    // console.log(meshNormalArrayZ);

    for (var i = 0; i < (n + 1) ** 2; i++) {
        getVertexNormal(i, n, meshNormalArrayX, meshNormalArrayY, meshNormalArrayZ, normalArray)
    }




}

/**
 * Input position, get the X index.
 * @param {number} p
 * @return index X
 */
function getIndexX(p) {
    return p * 3;
}

/**
 * Input position, get the Y index.
 * @param {number} p
 * @return index Y
 */
function getIndexY(p) {
    return p * 3 + 1;
}

/**
 * Input position, get the Z index.
 * @param {number} p
 * @return index Z
 */
function getIndexZ(p) {
    return p * 3 + 2;
}

/**
 * Calculate the cross product of two vectors.
 * @param {array} l1 line 1.
 * @param {array} l2 line 2.
 * @return the cross product.
 */
function crossProduct(l1, l2) {
    var ret = [];
    ret.push(l1[1] * l2[2] - l2[1] * l1[2]);
    ret.push(l1[2] * l2[0] - l2[2] * l1[0]);
    ret.push(l1[0] * l2[1] - l2[0] * l1[1]);

    return ret;
}

/**
 * get the vertex's normal.
 * @param {number} p
 * @param {number} n
 * @param {array} mnX
 * @param {array} mnY
 * @param {array} mnZ
 * @param {array} normalArray
 */
function getVertexNormal(p, n, mnX, mnY, mnZ, normalArray) {
    vertexNormal = [];
    if (p == 0) { // bottom-left corner
        var vertexNormal = single(0, mnX, mnY, mnZ);
        vertexNormal = normalization(vertexNormal);
        normalArray[getIndexX(p)] = vertexNormal[0];
        normalArray[getIndexY(p)] = vertexNormal[1];
        normalArray[getIndexZ(p)] = vertexNormal[2];

    } else if (p == n) { // bottom-right corner
        var vertexNormal = add2(n * 2 - 2, n * 2 - 1, mnX, mnY, mnZ);
        vertexNormal = normalization(vertexNormal);
        normalArray[getIndexX(p)] = vertexNormal[0];
        normalArray[getIndexY(p)] = vertexNormal[1];
        normalArray[getIndexZ(p)] = vertexNormal[2];

    } else if (p == n * (n + 1)) { // top-left corner
        var vertexNormal = add2(n * (n - 1) * 2, n * (n - 1) * 2 + 1, mnX, mnY, mnZ);
        vertexNormal = normalization(vertexNormal);
        normalArray[getIndexX(p)] = vertexNormal[0];
        normalArray[getIndexY(p)] = vertexNormal[1];
        normalArray[getIndexZ(p)] = vertexNormal[2];

    } else if (p == (n + 1) ** 2 - 1) { // top-right corner
        var vertexNormal = single(n ** 2 * 2 - 1, mnX, mnY, mnZ);
        vertexNormal = normalization(vertexNormal);
        normalArray[getIndexX(p)] = vertexNormal[0];
        normalArray[getIndexY(p)] = vertexNormal[1];
        normalArray[getIndexZ(p)] = vertexNormal[2];

    } else if (p < n) { // bottom row
        var vertexNormal = add3((p - 1) * 2,
                                (p - 1) * 2 + 1,
                                (p - 1) * 2 + 2,
                                mnX, mnY, mnZ);
        vertexNormal = normalization(vertexNormal);
        normalArray[getIndexX(p)] = vertexNormal[0];
        normalArray[getIndexY(p)] = vertexNormal[1];
        normalArray[getIndexZ(p)] = vertexNormal[2];

    } else if (p > n * (n + 1)) { // top row
        var vertexNormal = add3((n * n - ((n + 1) ** 2 - p)) * 2 + 1,
                                (n * n - ((n + 1) ** 2 - p)) * 2 + 2,
                                (n * n - ((n + 1) ** 2 - p)) * 2 + 3,
                                mnX, mnY, mnZ);
        vertexNormal = normalization(vertexNormal);
        normalArray[getIndexX(p)] = vertexNormal[0];
        normalArray[getIndexY(p)] = vertexNormal[1];
        normalArray[getIndexZ(p)] = vertexNormal[2];

    } else if (p % (n + 1) == 0) { // left column
        var vertexNormal = add3(p / (n + 1) * n * 2 - 2 * n,
                                p / (n + 1) * n * 2 - 2 * n + 1,
                                p / (n + 1) * n * 2,
                                mnX, mnY, mnZ);
        vertexNormal = normalization(vertexNormal);
        normalArray[getIndexX(p)] = vertexNormal[0];
        normalArray[getIndexY(p)] = vertexNormal[1];
        normalArray[getIndexZ(p)] = vertexNormal[2];

    } else if ((p + 1) % (n + 1) == 0) { // right column
        var vertexNormal = add3((p - n) / (n + 1) * n * 2 - 1,
                                (p + 1) / (n + 1) * n * 2 - 2,
                                (p + 1) / (n + 1) * n * 2 - 1,
                                mnX, mnY, mnZ);
        vertexNormal = normalization(vertexNormal);
        normalArray[getIndexX(p)] = vertexNormal[0];
        normalArray[getIndexY(p)] = vertexNormal[1];
        normalArray[getIndexZ(p)] = vertexNormal[2];

    } else { // internal vertices
        var vertexNormal = add6(p % (n + 1) * 2 + Math.floor(p / (n + 1)) * n * 2 - n * 2 - 1,
                                p % (n + 1) * 2 + Math.floor(p / (n + 1)) * n * 2 - n * 2,
                                p % (n + 1) * 2 + Math.floor(p / (n + 1)) * n * 2 - n * 2 + 1,
                                p % (n + 1) * 2 + Math.floor(p / (n + 1)) * n * 2 - 2,
                                p % (n + 1) * 2 + Math.floor(p / (n + 1)) * n * 2 - 1,
                                p % (n + 1) * 2 + Math.floor(p / (n + 1)) * n * 2,
                                mnX, mnY, mnZ);
        vertexNormal = normalization(vertexNormal);
        normalArray[getIndexX(p)] = vertexNormal[0];
        normalArray[getIndexY(p)] = vertexNormal[1];
        normalArray[getIndexZ(p)] = vertexNormal[2];
    }
}

/**
 * normalization.
 * @param vertexNormal
 * @return normalized vector.
 */
function normalization(vertexNormal) {
    normalFactor = 1 / Math.sqrt(vertexNormal[0] ** 2
                              + vertexNormal[1] ** 2 + vertexNormal[2] ** 2);
    vertexNormal[0] = vertexNormal[0] * normalFactor;
    vertexNormal[1] = vertexNormal[1] * normalFactor;
    vertexNormal[2] = vertexNormal[2] * normalFactor;

    return vertexNormal;
}

/**
 * one vector.
 * @param {number} p
 * @param {array} mnX
 * @param {array} mnY
 * @param {array} mnZ
 * @param {array} normalArray
 * @return get the normal vector.
 */
function single(p, mnX, mnY, mnZ) {
    ret = [];
    ret.push(mnX[p]);
    ret.push(mnY[p]);
    ret.push(mnZ[p]);

    return ret;
}

/**
 * two vectors.
 * @param {number} p
 * @param {array} mnX
 * @param {array} mnY
 * @param {array} mnZ
 * @param {array} normalArray
 * @return get the normal vector.
 */
function add2(p1, p2, mnX, mnY, mnZ) {
    ret = [];
    ret.push(mnX[p1] + mnX[p2]);
    ret.push(mnY[p1] + mnY[p2]);
    ret.push(mnZ[p1] + mnZ[p2]);

    return ret;
}

/**
 * three vectors.
 * @param {number} p
 * @param {array} mnX
 * @param {array} mnY
 * @param {array} mnZ
 * @param {array} normalArray
 * @return get the normal vector.
 */
function add3(p1, p2, p3, mnX, mnY, mnZ) {
    ret = [];
    ret.push(mnX[p1] + mnX[p2] + mnX[p3]);
    ret.push(mnY[p1] + mnY[p2] + mnY[p3]);
    ret.push(mnZ[p1] + mnZ[p2] + mnZ[p3]);

    return ret;
}

/**
 * six vectors.
 * @param {number} p
 * @param {array} mnX
 * @param {array} mnY
 * @param {array} mnZ
 * @param {array} normalArray
 * @return get the normal vector.
 */
function add6(p1, p2, p3, p4, p5, p6, mnX, mnY, mnZ) {
    ret = [];
    ret.push(mnX[p1] + mnX[p2] + mnX[p3] + mnX[p4] + mnX[p5] + mnX[p6]);
    ret.push(mnY[p1] + mnY[p2] + mnY[p3] + mnY[p4] + mnY[p5] + mnY[p6]);
    ret.push(mnZ[p1] + mnZ[p2] + mnZ[p3] + mnZ[p4] + mnZ[p5] + mnZ[p6]);

    return ret;
}


/**
 * Generates line values from faces in faceArray
 * @param {Array} faceArray array of faces for triangles
 * @param {Array} lineArray array of normals for triangles, storage location after generation
 */
function generateLinesFromIndexedTriangles(faceArray, lineArray) {
    numTris = faceArray.length / 3;
    for(var f = 0; f < numTris; f++) {
        var fid = f * 3;
        lineArray.push(faceArray[fid]);
        lineArray.push(faceArray[fid + 1]);

        lineArray.push(faceArray[fid + 1]);
        lineArray.push(faceArray[fid + 2]);

        lineArray.push(faceArray[fid + 2]);
        lineArray.push(faceArray[fid]);
    }
    console.log(faceArray);
}
