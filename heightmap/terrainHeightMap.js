
var imageObj = new Image();
imageObj.onload = function() {
    getHeightFromGrayscale(this);
};
imageObj.src = 'images/heightHMBIG.png';

/**
 * Iteratively generate terrain from numeric inputs
 * @param {number} n
 * @param {number} minX Minimum X value
 * @param {number} maxX Maximum X value
 * @param {number} minY Minimum Y value
 * @param {number} maxY Maximum Y value
 * @param {Array} vertexArray Array that will contain vertices generated
 * @param {Array} faceArray Array that will contain faces generated
 * @param {Array} normalArray Array that will contain normals generated
 * @return {number}
 */
function terrainReader(n, minX, maxX, minY, maxY, vertexArray, faceArray, normalArray, colorArray) {
    if (Math.log2(n) % 1 != 0) {
        alert("Not a valid edge length!!");
        return;
    }

    var heightArray = [];
    var countArray = [];
    var imgEdge = Math.sqrt(heightData.length);

    for (var i = 0; i <= n*n; i++) {
        heightArray.push(0);
        countArray.push(0);
    }

    for (var j = 0; j < imgEdge; j++) {
        for (var i = 0; i < imgEdge; i++) {
            heightArray[j*3075+i*2] += heightData[j*512+i];
            heightArray[j*3075+i*2+1] += heightData[j*512+i];
            heightArray[j*3075+i*2+2] += heightData[j*512+i];

            heightArray[j*3075+i*2+1025] += heightData[j*512+i];
            heightArray[j*3075+i*2+1026] += heightData[j*512+i];
            heightArray[j*3075+i*2+1027] += heightData[j*512+i];

            heightArray[j*3075+i*2+2050] += heightData[j*512+i];
            heightArray[j*3075+i*2+2051] += heightData[j*512+i];
            heightArray[j*3075+i*2+2052] += heightData[j*512+i];


            countArray[j*3075+i*2] += 1;
            countArray[j*3075+i*2+1] += 1;
            countArray[j*3075+i*2+2] += 1;

            countArray[j*3075+i*2+1025] += 1;
            countArray[j*3075+i*2+1026] += 1;
            countArray[j*3075+i*2+1027] += 1;

            countArray[j*3075+i*2+2050] += 1;
            countArray[j*3075+i*2+2051] += 1;
            countArray[j*3075+i*2+2052] += 1;
        }
    }

    for (var i = 0; i < heightArray.length; i++) {
        heightArray[i] = heightArray[i] / countArray[i];
        if (heightArray[i] == 0) {
        }
    }


    var deltaX = (maxX - minX) / n;
    var deltaY = (maxY - minY) / n;
    for(var j = 0; j <= n; j++) {
        for(var i = 0; i <= n; i++) {
            vertexArray.push(minY + deltaY * i);
            vertexArray.push(minX + deltaX * j);
            vertexArray.push(heightArray[j*n+i]/700.0);

            normalArray.push(0);
            normalArray.push(0);
            normalArray.push(1);
        }
    }

    generateColor(n, colorArray, vertexArray);
    updateNormalArray(n, vertexArray, normalArray);

    var numT = 0;
    for(var i = 0; i < n; i++) {
        for(var j = 0; j < n; j++) {
            var vid = i * (n + 1) + j;

            faceArray.push(vid);
            faceArray.push(vid + n + 1);
            faceArray.push(vid + 1);

            faceArray.push(vid + 1);
            faceArray.push(vid + n + 1);
            faceArray.push(vid + 1 + n + 1);
            numT += 2;
        }
    }

    var heightPoint = [];
    for (var i = 0; i < vertexArray.length/3; i++) {
        heightPoint.push(vertexArray[i*3]);
    }
    // console.log("heightArray", heightArray)
     console.log("heightData", heightData)
    // console.log("pixelData", pixelData)
    return numT;
}


/**
 * get normalize height array.
 * @param n
 */
function getHeightArray(n) {
    for (var i = 0; i <= n*n; i++) {
        heightArray.push(0);
        countArray.push(0);
    }

    for (var j = 0; j < imgEdge; j++) {
        for (var i = 0; i < imgEdge; i++) {
            heightArray[i*3075+j*2] += heightData[i*512+j];
            heightArray[i*3075+j*2+1] += heightData[i*512+j];
            heightArray[i*3075+j*2+2] += heightData[i*512+j];

            heightArray[i*3075+j*2+1025] += heightData[i*512+j];
            heightArray[i*3075+j*2+1026] += heightData[i*512+j];
            heightArray[i*3075+j*2+1027] += heightData[i*512+j];

            heightArray[i*3075+j*2+2050] += heightData[i*512+j];
            heightArray[i*3075+j*2+2051] += heightData[i*512+j];
            heightArray[i*3075+j*2+2052] += heightData[i*512+j];


            countArray[i*3075+j*2] += 1;
            countArray[i*3075+j*2+1] += 1;
            countArray[i*3075+j*2+2] += 1;

            countArray[i*3075+j*2+1025] += 1;
            countArray[i*3075+j*2+1026] += 1;
            countArray[i*3075+j*2+1027] += 1;

            countArray[i*3075+j*2+2050] += 1;
            countArray[i*3075+j*2+2051] += 1;
            countArray[i*3075+j*2+2052] += 1;
        }
    }

    for (var i = 0; i < heightArray.length; i++) {
        heightArray[i] = heightArray[i] / countArray[i];
    }
}

/**
 * get pixel data from the target PNG file.
 * @param imageObj
 */
function drawImage(imageObj) {
    var canvas = document.getElementById('myCanvas');
    var context = canvas.getContext('2d');
    var imageX = 0;
    var imageY = 0;
    var imageWidth = imageObj.width;
    var imageHeight = imageObj.height;

    context.drawImage(imageObj, imageX, imageY);

    var imageData = context.getImageData(imageX, imageY, imageWidth, imageHeight);
    pixelData = imageData.data;
}


/**
 * tranfer the pixel array to height array.
 * @param imageObj
 */
function getHeightFromGrayscale(imageObj) {
    drawImage(imageObj);
    for (var i = 0; i < pixelData.length / 4; i++) {
        heightData[i] = pixelData[i*4];
    }
}
