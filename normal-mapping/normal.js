
var gl;
var canvas;

var shaderProgram;

// arrays we need for sphere
var vertexArray = [];
var normalArray = [];
var faceArray = [];
var textureCoordArray = [];
var _tangentArray = [];
var _bitangentArray = [];
var _normalArray = [];

// buffers
var tVertexPositionBuffer;
var tVertexNormalBuffer;
var tVertexTextureCoordBuffer;
var tVertexIndexBuffer;
var tsTangentBuffer;
var tsBitangentBuffer;
var tsNormalBuffer;

// texture
var sphereTexture;

var num;

// State the points
var eyePt = vec3.fromValues(0.0, 4.0, 16.0);
var viewDir = vec3.fromValues(0.0, -0.5, -1.0);
var up = vec3.fromValues(0.0, 1.0, 0.0);
var viewPt = vec3.fromValues(0.0, 0.0, 0.0);
var lightDir = vec3.fromValues(-10.0, -10.0, -10.0);

// Create ModelView matrix
var mvMatrix = mat4.create();

//Create Projection matrix
var pMatrix = mat4.create();

var mvMatrixStack = [];

// serve for rotation.
var modelXRotationRadians = degToRad(0);
var modelYRotationRadians = degToRad(0);


/**
 * Sends Modelview matrix to shader
 */
function uploadModelViewMatrixToShader() {
    gl.uniformMatrix4fv(shaderProgram.mvMatrixUniform, false, mvMatrix);
}

/**
 * Sends projection matrix to shader
 */
function uploadProjectionMatrixToShader() {
    gl.uniformMatrix4fv(shaderProgram.pMatrixUniform, false, pMatrix);
}

/**
 * Pushes matrix onto modelview matrix stack
 */
function mvPushMatrix() {
    var copy = mat4.clone(mvMatrix);
    mvMatrixStack.push(copy);
}


/**
 * Pops matrix off of modelview matrix stack
 */
function mvPopMatrix() {
    if (mvMatrixStack.length == 0) {
      throw "Invalid popMatrix!";
    }
    mvMatrix = mvMatrixStack.pop();
}

/**
 * Sends projection/modelview matrices to shader
 */
function setMatrixUniforms() {
    uploadModelViewMatrixToShader();
    uploadProjectionMatrixToShader();
}

/**
 * Sends light information to the shader
 * @param {Float32Array} loc Location of light source
 * @param {Float32Array} a Ambient light strength
 * @param {Float32Array} d Diffuse light strength
 * @param {Float32Array} s Specular light strength
 */
function uploadLightsToShader(loc,a,d,s) {
    gl.useProgram(shaderProgram);
    gl.uniform3fv(shaderProgram.uniformLightPositionLoc, loc);
    gl.uniform3fv(shaderProgram.ambientColorUniform, a);
    gl.uniform3fv(shaderProgram.lightingDirectionUniform, d);
    gl.uniform3fv(shaderProgram.directionalColorUniform, s);
}

/**
 * Translates degrees to radians
 * @param {Number} degrees Degree input to function
 * @return {Number} The radians that correspond to the degree input
 */
function degToRad(degrees) {
    return degrees * Math.PI / 180;
}

/**
 * Creates a context for WebGL
 * @param {element} canvas WebGL canvas
 * @return {Object} WebGL context
 */
function createGLContext(canvas) {
    var names = ["webgl", "experimental-webgl"];
    var context = null;
    for (var i=0; i < names.length; i++) {
        try {
            context = canvas.getContext(names[i]);
        } catch(e) {}
        if (context) {
            break;
        }
    }
    if (context) {
        context.viewportWidth = canvas.width;
        context.viewportHeight = canvas.height;
    } else {
        alert("Failed to create WebGL context!");
    }
    return context;
}

/**
 * Loads Shaders
 * @param {string} id ID string for shader to load. Either vertex shader/fragment shader
 */
function loadShaderFromDOM(id) {
    var shaderScript = document.getElementById(id);

    // If we don't find an element with the specified id
    // we do an early exit
    if (!shaderScript) {
        return null;
    }

    // Loop through the children for the found DOM element and
    // build up the shader source code as a string
    var shaderSource = "";
    var currentChild = shaderScript.firstChild;
    while (currentChild) {
        if (currentChild.nodeType == 3) { // 3 corresponds to TEXT_NODE
            shaderSource += currentChild.textContent;
        }
        currentChild = currentChild.nextSibling;
    }

    var shader;
    if (shaderScript.type == "x-shader/x-fragment") {
        shader = gl.createShader(gl.FRAGMENT_SHADER);
    } else if (shaderScript.type == "x-shader/x-vertex") {
        shader = gl.createShader(gl.VERTEX_SHADER);
    } else {
        return null;
    }

    gl.shaderSource(shader, shaderSource);
    gl.compileShader(shader);

    if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
        alert(gl.getShaderInfoLog(shader));
        return null;
    }
    return shader;
}

/**
 * Setup the fragment and vertex shaders
 */
function setupShaders() {
    vertexShader = loadShaderFromDOM("shader-vs");
    fragmentShader = loadShaderFromDOM("shader-fs");

    shaderProgram = gl.createProgram();
    gl.attachShader(shaderProgram, vertexShader);
    gl.attachShader(shaderProgram, fragmentShader);
    gl.linkProgram(shaderProgram);

    if (!gl.getProgramParameter(shaderProgram, gl.LINK_STATUS)) {
        alert("Failed to setup shaders");
    }

    gl.useProgram(shaderProgram);


    shaderProgram.vertexPositionAttribute = gl.getAttribLocation(shaderProgram, "aVertexPosition");
    gl.enableVertexAttribArray(shaderProgram.vertexPositionAttribute);

    shaderProgram.textureCoordAttribute = gl.getAttribLocation(shaderProgram, "aTextureCoord");
    gl.enableVertexAttribArray(shaderProgram.textureCoordAttribute);

    shaderProgram.vertexNormalAttribute = gl.getAttribLocation(shaderProgram, "aVertexNormal");
    gl.enableVertexAttribArray(shaderProgram.vertexNormalAttribute);

    shaderProgram.pMatrixUniform = gl.getUniformLocation(shaderProgram, "uPMatrix");
    shaderProgram.mvMatrixUniform = gl.getUniformLocation(shaderProgram, "uMVMatrix");
    shaderProgram.nMatrixUniform = gl.getUniformLocation(shaderProgram, "uNMatrix");
    shaderProgram.samplerUniform = gl.getUniformLocation(shaderProgram, "uSampler");
    shaderProgram.useLightingUniform = gl.getUniformLocation(shaderProgram, "uUseLighting");
    shaderProgram.ambientColorUniform = gl.getUniformLocation(shaderProgram, "uAmbientColor");
    shaderProgram.lightingDirectionUniform = gl.getUniformLocation(shaderProgram, "uLightingDirection");
    shaderProgram.directionalColorUniform = gl.getUniformLocation(shaderProgram, "uDirectionalColor");
    shaderProgram.uniformLightPositionLoc = gl.getUniformLocation(shaderProgram, "uLightPosition");
}


/**
 * Creates texture for application to cube.
 */
function setupTextures() {
    sphereTexture = gl.createTexture();
    sphereTexture.image = new Image();
    sphereTexture.image.onload = function () {
        handleTextureLoaded(sphereTexture);
    }

    sphereTexture.image.src = "blocks_normal.jpg";
}

/**
 * @param {number} value Value to determine whether it is a power of 2
 * @return {boolean} Boolean of whether value is a power of 2
 */
function isPowerOf2(value) {
    return (value & (value - 1)) == 0;
}

/**
 * Texture handling. Generates mipmap and sets texture parameters.
 * @param {Object} image Image for cube application
 * @param {Object} texture Texture for cube application
 */
function handleTextureLoaded(texture) {
    gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
    gl.bindTexture(gl.TEXTURE_2D, texture);
    gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, texture.image);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR_MIPMAP_NEAREST);
    gl.generateMipmap(gl.TEXTURE_2D);

    gl.bindTexture(gl.TEXTURE_2D, null);
}

/**
 * Populate buffers with data
 */
function setupBuffers() {
    buildSphere();

    // num = sphereFromSubdivision(6, vertexArray, normalArray);
    //console.log(vertexArray)
    // getTextureCoord();

    tVertexPositionBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, tVertexPositionBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertexArray), gl.STATIC_DRAW);
    tVertexPositionBuffer.itemSize = 3;
    tVertexPositionBuffer.numItems = vertexArray.length / 3;

    tVertexTextureCoordBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, tVertexTextureCoordBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(textureCoordArray), gl.STATIC_DRAW);
    tVertexTextureCoordBuffer.itemSize = 2;
    tVertexTextureCoordBuffer.numItems = textureCoordArray.length / 2;

    tVertexNormalBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, tVertexNormalBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(normalArray), gl.STATIC_DRAW);
    tVertexNormalBuffer.itemSize = 3;
    tVertexNormalBuffer.numItems = normalArray.length / 3;

    tsTangentBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, tsTangentBuffer);
    gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Float32Array(_tangentArray), gl.STATIC_DRAW);
    tsTangentBuffer.itemSize = 1;
    tsTangentBuffer.numItems = _tangentArray.length;

    tsBitangentBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, tsBitangentBuffer);
    gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Float32Array(_bitangentArray), gl.STATIC_DRAW);
    tsBitangentBuffer.itemSize = 1;
    tsBitangentBuffer.numItems = _bitangentArray.length;

    tsNormalBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, tsNormalBuffer);
    gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Float32Array(_normalArray), gl.STATIC_DRAW);
    tsNormalBuffer.itemSize = 1;
    tsNormalBuffer.numItems = _normalArray.length;

    tVertexIndexBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, tVertexIndexBuffer);
    gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint16Array(faceArray), gl.STATIC_DRAW);
    tVertexIndexBuffer.itemSize = 1;
    tVertexIndexBuffer.numItems = faceArray.length;
}

function getTextureCoord() {
    for (var i = 0; i < vertexArray.length; i += 3) {
        textureCoordArray.push(Math.atan2(vertexArray[i], vertexArray[i+2]) / (2*Math.PI) + 0.5);
        textureCoordArray.push(vertexArray[i+1] * 0.5 + 0.5);
    }
}

function buildSphere() {
    var latitudeBands = 30;
    var longitudeBands = 30;
    var radius = 2;

    for (var latNumber = 0; latNumber <= latitudeBands; latNumber++) {
        var theta = latNumber * Math.PI / latitudeBands;
        var sinTheta = Math.sin(theta);
        var cosTheta = Math.cos(theta);

        for (var longNumber = 0; longNumber <= longitudeBands; longNumber++) {
            var phi = longNumber * 2 * Math.PI / longitudeBands;
            var sinPhi = Math.sin(phi);
            var cosPhi = Math.cos(phi);

            var x = cosPhi * sinTheta;
            var y = cosTheta;
            var z = sinPhi * sinTheta;
            var u = 1 - (longNumber / longitudeBands);
            var v = 1 - (latNumber / latitudeBands);

            normalArray.push(x);
            normalArray.push(y);
            normalArray.push(z);
            textureCoordArray.push(u);
            textureCoordArray.push(v);
            vertexArray.push(radius*x);
            vertexArray.push(radius*y);
            vertexArray.push(radius*z);
        }
    }

    console.log("texture coord", textureCoordArray)
    console.log("normal array", normalArray)
    console.log("vertex array", vertexArray)

    for (var latNumber = 0; latNumber < latitudeBands; latNumber++) {
        for (var longNumber = 0; longNumber < longitudeBands; longNumber++) {
            var first = (latNumber * (longitudeBands + 1)) + longNumber;
            var second = first + longitudeBands + 1;
            faceArray.push(first);
            faceArray.push(second);
            faceArray.push(first+1);

            faceArray.push(second);
            faceArray.push(second+1);
            faceArray.push(first+1);
        }
    }
    console.log("face array", faceArray)

    findTangentSpace();

    console.log("_tangent", _tangentArray)
    console.log("_bitangent", _bitangentArray)
    console.log("_normal", _normalArray)
}


function findTangentSpace() {
    for (var i = 0; i < faceArray.length*3; i++) {
        _normalArray.push(0.0);
        _tangentArray.push(0.0);
        _bitangentArray.push(0.0);
    }

    for (var i = 0; i < faceArray.length; i += 3) {
        var point0 = faceArray[i];
        var point1 = faceArray[i+1];
        var point2 = faceArray[i+2];

        computeSingleTangentSpace(point0, point1, point2, i);
    }
}

function computeSingleTangentSpace(p0, p1, p2, i) {
    // vertices.
    var x0 = vertexArray[p0*3];
    var y0 = vertexArray[p0*3+1];
    var z0 = vertexArray[p0*3+2];

    var x1 = vertexArray[p1*3];
    var y1 = vertexArray[p1*3+1];
    var z1 = vertexArray[p1*3+2];

    var x2 = vertexArray[p2*3];
    var y2 = vertexArray[p2*3+1];
    var z2 = vertexArray[p2*3+2];

    var v1x = x1 - x0;
    var v1y = y1 - y0;
    var v1z = z1 - z0;

    var v2x = x2 - x0;
    var v2y = y2 - y0;
    var v2z = z2 - z0;
    //-------------------------------------

    // texture coord.
    var u0 = textureCoordArray[p0*2];
    var v0 = textureCoordArray[p0*2+1];

    var u1 = textureCoordArray[p1*2];
    var v1 = textureCoordArray[p1*2+1];

    var u2 = textureCoordArray[p2*2];
    var v2 = textureCoordArray[p2*2+1];

    var u1u = u1 - u0;
    var u1v = v1 - v0;

    var u2u = u2 - u0;
    var u2v = v2 - v0;
    //--------------------------------------

    // determinant
    var det = u1u * u2v - u2u * u1v;
    //--------------------------------------

    // tangent vectors
    var tangent_x = (v1x * u2v - v2x * u1v) / det;
    var tangent_y = (v1y * u2v - v2y * u1v) / det;
    var tangent_z = (v1z * u2v - v2z * u1v) / det;

    _tangentArray[i*3] = tangent_x;
    _tangentArray[i*3+1] = tangent_y;
    _tangentArray[i*3+2] = tangent_z;

    _tangentArray[i*3+3] = tangent_x;
    _tangentArray[i*3+4] = tangent_y;
    _tangentArray[i*3+5] = tangent_z;

    _tangentArray[i*3+6] = tangent_x;
    _tangentArray[i*3+7] = tangent_y;
    _tangentArray[i*3+8] = tangent_z;
    //-----------------------------------------

    // bitangent vectors
    var bitangent_x = (-v1x * u2u + v2x * u1u) / det;
    var bitangent_y = (-v1y * u2u + v2y * u1u) / det;
    var bitangent_z = (-v1z * u2u + v2z * u1u) / det;

    _bitangentArray[i*3] = bitangent_x;
    _bitangentArray[i*3+1] = bitangent_y;
    _bitangentArray[i*3+2] = bitangent_z;

    _bitangentArray[i*3+3] = bitangent_x;
    _bitangentArray[i*3+4] = bitangent_y;
    _bitangentArray[i*3+5] = bitangent_z;

    _bitangentArray[i*3+6] = bitangent_x;
    _bitangentArray[i*3+7] = bitangent_y;
    _bitangentArray[i*3+8] = bitangent_z;
    //-------------------------------------------

    // normal vectors
    var normal_x = bitangent_y * tangent_z - bitangent_z * tangent_y;
    var normal_y = bitangent_z * tangent_x - bitangent_x * tangent_z;
    var normal_z = bitangent_x * tangent_y - bitangent_y * tangent_x;

    _normalArray[i*3] = normal_x;
    _normalArray[i*3+1] = normal_y;
    _normalArray[i*3+2] = normal_z;

    _normalArray[i*3+3] = normal_x;
    _normalArray[i*3+4] = normal_y;
    _normalArray[i*3+5] = normal_z;

    _normalArray[i*3+6] = normal_x;
    _normalArray[i*3+7] = normal_y;
    _normalArray[i*3+8] = normal_z;
}

/**
 * Draw a cube based on buffers.
 */
function drawSphere(){
    gl.activeTexture(gl.TEXTURE0);
    gl.bindTexture(gl.TEXTURE_2D, sphereTexture);
    gl.uniform1i(shaderProgram.samplerUniform, 0);

    gl.bindBuffer(gl.ARRAY_BUFFER, tVertexPositionBuffer);
    gl.vertexAttribPointer(shaderProgram.vertexPositionAttribute, tVertexPositionBuffer.itemSize, gl.FLOAT, false, 0, 0);

    gl.bindBuffer(gl.ARRAY_BUFFER, tVertexTextureCoordBuffer);
    gl.vertexAttribPointer(shaderProgram.textureCoordAttribute, tVertexTextureCoordBuffer.itemSize, gl.FLOAT, false, 0, 0);

    gl.bindBuffer(gl.ARRAY_BUFFER, tVertexNormalBuffer);
    gl.vertexAttribPointer(shaderProgram.vertexNormalAttribute, tVertexNormalBuffer.itemSize, gl.FLOAT, false, 0, 0);

    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, tVertexIndexBuffer);
    setMatrixUniforms();
    gl.drawElements(gl.TRIANGLES, tVertexIndexBuffer.numItems, gl.UNSIGNED_SHORT, 0);
}

/**
 * Draw call that applies matrix transformations to cube
 */
function draw() {
    var transformVec = vec3.create();
    gl.viewport(0, 0, gl.viewportWidth, gl.viewportHeight);
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    mat4.perspective(pMatrix, 45, gl.viewportWidth / gl.viewportHeight, 0.1, 100.0);

    vec3.add(viewPt, eyePt, viewDir);
    mat4.lookAt(mvMatrix, eyePt, viewPt, up);

    keyListener();

    mvPushMatrix();
    mat4.translate(mvMatrix, mvMatrix, [0, 0, 10]);
    mat4.rotateX(mvMatrix, mvMatrix, modelXRotationRadians);
    mat4.rotateY(mvMatrix, mvMatrix, modelYRotationRadians);
    setMatrixUniforms();
    drawSphere();
    uploadLightsToShader(lightDir, [0.8,0.8,0.8], [1.0,1.0,1.0], [1.0,1.0,1.0]);
    mvPopMatrix();
}


function requestCORSIfNotSameOrigin(img, url) {
    if ((new URL(url)).origin !== window.location.origin) {
        img.crossOrigin = "";
    }
}


/**
 * Listen the key press to do the rotation.
 */
function keyListener() {
    var speed = 0.02;

    window.addEventListener("keydown", function (event) {
        if (event.defaultPrevented) {
            return; // Do nothing if the event was already processed
        }

        switch (event.key) {
            // left rotation.
            case "ArrowLeft":
            modelYRotationRadians += speed;
            break;

            // right rotation
            case "ArrowRight":
            modelYRotationRadians -= speed;
            break;

            // upward rotation.
            case "ArrowUp":
            modelXRotationRadians += speed;
            break;

            // downward rotation
            case "ArrowDown":
            modelXRotationRadians -= speed;
            break;

            default:
            return; // Quit when this doesn't handle the key event.
        }

        // Cancel the default action to avoid it being handled twice
        event.preventDefault();
    }, true);
}


/**
 * Startup function called from html code to start program.
 */
function startup() {
    canvas = document.getElementById("myGLCanvas");
    gl = createGLContext(canvas);
    gl.clearColor(0.0, 0.0, 0.0, 1.0);
    gl.enable(gl.DEPTH_TEST);

    setupShaders();
    setupBuffers();
    setupTextures();
    tick();
}

/**
 * Tick called for every animation frame.
 */
function tick() {
    requestAnimFrame(tick);
    draw();
}
