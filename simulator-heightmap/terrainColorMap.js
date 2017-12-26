
// store the raw color data for the terrain.
var pixelColorData = [];

var colorObj = new Image();
colorObj.onload = function() {
    getColorFromColorscale(this);
};
colorObj.src = 'images/colorHMBIG.png';

/**
 * Iteratively generate terrain from numeric inputs
 * @param {number} n
 * @param {number} minX Minimum X value
 * @param {number} maxX Maximum X value
 * @param {number} minY Minimum Y value
 * @param {number} maxY Maximum Y value
 * @param {Array} colorArray Array that will contain colors
 */
function colorReader(n, minX, maxX, minY, maxY, colorArray) {
    if (Math.log2(n) % 1 != 0) {
        alert("Not a valid edge length!!");
        return;
    }

    var countArray = [];
    var tempColorArray = [];

    for (var i = 0; i < (n+1)*(n+1)*3; i++) {
        tempColorArray.push(0);
        countArray.push(0);
    }

    for (var j = 0; j < 1024; j++) {
        for (var i = 0; i < 1024; i++) {
            var z = 1023 - j;
            // for R
            tempColorArray[(z*1025+i)*3] += colorData[(j*1024+i)*3];
            tempColorArray[(z*1025+i+1)*3] += colorData[(j*1024+i)*3];
            tempColorArray[(z*1025+i+1025)*3] += colorData[(j*1024+i)*3];
            tempColorArray[(z*1025+i+1026)*3] += colorData[(j*1024+i)*3];

            countArray[(z*1025+i)*3] += 1;
            countArray[(z*1025+i+1)*3] += 1;
            countArray[(z*1025+i+1025)*3] += 1;
            countArray[(z*1025+i+1026)*3] += 1;

            // for G
            tempColorArray[(z*1025+i)*3+1] += colorData[(j*1024+i)*3+1];
            tempColorArray[(z*1025+i+1)*3+1] += colorData[(j*1024+i)*3+1];
            tempColorArray[(z*1025+i+1025)*3+1] += colorData[(j*1024+i)*3+1];
            tempColorArray[(z*1025+i+1026)*3+1] += colorData[(j*1024+i)*3+1];

            countArray[(z*1025+i)*3+1] += 1;
            countArray[(z*1025+i+1)*3+1] += 1;
            countArray[(z*1025+i+1025)*3+1] += 1;
            countArray[(z*1025+i+1026)*3+1] += 1;

            // for B
            tempColorArray[(z*1025+i)*3+2] += colorData[(j*1024+i)*3+2];
            tempColorArray[(z*1025+i+1)*3+2] += colorData[(j*1024+i)*3+2];
            tempColorArray[(z*1025+i+1025)*3+2] += colorData[(j*1024+i)*3+2];
            tempColorArray[(z*1025+i+1026)*3+2] += colorData[(j*1024+i)*3+2];

            countArray[(z*1025+i)*3+2] += 1;
            countArray[(z*1025+i+1)*3+2] += 1;
            countArray[(z*1025+i+1025)*3+2] += 1;
            countArray[(z*1025+i+1026)*3+2] += 1;
        }
    }

    for (var i = 0; i < tempColorArray.length; i++) {
        tempColorArray[i] = tempColorArray[i] / countArray[i];
        colorArray.push(tempColorArray[i]);
    }
}


/**
 * get pixel data from the target PNG file.
 * @param colorObj
 */
function drawColorImage(colorObj) {
    var canvas = document.getElementById('colorMap');
    var context = canvas.getContext('2d');
    var imageX = 0;
    var imageY = 0;
    var imageWidth = colorObj.width;
    var imageHeight = colorObj.height;

    context.drawImage(colorObj, imageX, imageY);

    var imageData = context.getImageData(imageX, imageY, imageWidth, imageHeight);
    pixelColorData = imageData.data;
}


/**
 * tranfer the pixel array to height array.
 * @param colorObj
 */
function getColorFromColorscale(colorObj) {
    drawColorImage(colorObj);
    for (var i = 0; i < pixelColorData.length; i++) {
        if ((i + 1) % 4 != 0) {
            colorData.push(pixelColorData[i] / 255.0);
        }
    }
}
