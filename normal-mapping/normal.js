
var gl;
var canvas;

var shaderProgram;

// arrays we need for sphere
var vertexArray = [];
var tangentArray = [];
var normalArray = [];
var bitangentArray = [];
var faceArray = [];
var textureCoordArray = [];

// buffers
var tVertexPositionBuffer;
var tVertexNormalBuffer;
var tVertexTangentBuffer;
var tVertexBitangentBuffer
var tVertexTextureCoordBuffer;

// texture
var sphereTexture;

var num;

// State the points
var eyePt = vec3.fromValues(0.0, 0.5, 13.0);
var viewDir = vec3.fromValues(0.0, -0.5, -1.0);
var up = vec3.fromValues(0.0, 1.0, 0.0);
var viewPt = vec3.fromValues(0.0, 0.0, 0.0);

// Create ModelView matrix
var mvMatrix = mat4.create();

// Create Projection matrix
var pMatrix = mat4.create();

// create Normal matrix
var nMatrix = mat4.create();

var mvMatrixStack = [];

// serve for rotation.
var modelXRotationRadians = degToRad(-20);
var modelYRotationRadians = degToRad(-90);


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
 * Generates and sends the normal matrix to the shader
 */
function uploadNormalMatrixToShader() {
    mat4.copy(nMatrix, mvMatrix);
    mat4.transpose(nMatrix, nMatrix);
    mat4.invert(nMatrix, nMatrix);
    gl.uniformMatrix4fv(shaderProgram.nMatrixUniform, false, nMatrix);
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
    uploadNormalMatrixToShader();
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
    gl.uniform3fv(shaderProgram.uniformLightPositionLoc, loc);
    gl.uniform4fv(shaderProgram.uniformAmbientLightColorLoc, a);
    gl.uniform4fv(shaderProgram.uniformDiffuseLightColorLoc, d);
    gl.uniform4fv(shaderProgram.uniformSpecularLightColorLoc, s);
}

/**
 * Sends material information to the shader
 * @param {Float32Array} a Material color under Ambient lights
 * @param {Float32Array} d Material color under Diffuse lights
 * @param {Float32Array} s Material color under Specular lights
 */
function uploadMaterialToShader(a,d,s) {
    gl.uniform4fv(shaderProgram.uniformAmbientMatColorLoc, a);
    gl.uniform4fv(shaderProgram.uniformDiffuseMatColorLoc, d);
    gl.uniform4fv(shaderProgram.uniformSpecularMatColorLoc, s);
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

    shaderProgram.vertexTangentAttribute = gl.getAttribLocation(shaderProgram, "aVertexTangent");
    gl.enableVertexAttribArray(shaderProgram.vertexTangentAttribute);

    shaderProgram.vertexBitangentAttribute = gl.getAttribLocation(shaderProgram, "aVertexBitangent");
    gl.enableVertexAttribArray(shaderProgram.vertexBitangentAttribute);

    shaderProgram.pMatrixUniform = gl.getUniformLocation(shaderProgram, "uPMatrix");
    shaderProgram.mvMatrixUniform = gl.getUniformLocation(shaderProgram, "uMVMatrix");
    shaderProgram.nMatrixUniform = gl.getUniformLocation(shaderProgram, "uNMatrix");
    shaderProgram.samplerUniform = gl.getUniformLocation(shaderProgram, "uSampler");
    shaderProgram.lightingDirectionUniform = gl.getUniformLocation(shaderProgram, "uLightingDirection");
    shaderProgram.uniformLightPositionLoc = gl.getUniformLocation(shaderProgram, "uLightPosition");

    shaderProgram.uniformAmbientLightColorLoc = gl.getUniformLocation(shaderProgram, "uAmbientLightColor");
    shaderProgram.uniformDiffuseLightColorLoc = gl.getUniformLocation(shaderProgram, "uDiffuseLightColor");
    shaderProgram.uniformSpecularLightColorLoc = gl.getUniformLocation(shaderProgram, "uSpecularLightColor");

    shaderProgram.uniformAmbientMatColorLoc = gl.getUniformLocation(shaderProgram, "uAmbientMatColor");
    shaderProgram.uniformDiffuseMatColorLoc = gl.getUniformLocation(shaderProgram, "uDiffuseMatColor");
    shaderProgram.uniformSpecularMatColorLoc = gl.getUniformLocation(shaderProgram, "uSpecularMatColor");
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

    sphereTexture.image.src = "609-normal.jpg";
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
    num = sphereFromSubdivision(6, vertexArray, normalArray);
    console.log("I have generated", num, "triangles to form this sphere.")

    // get texture coordinate.
    getTextureCoord();

    // find tangent space arrays.
    getTangentSpace();

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

    tVertexTangentBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, tVertexTangentBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(tangentArray), gl.STATIC_DRAW);
    tVertexTangentBuffer.itemSize = 3;
    tVertexTangentBuffer.numItems = tangentArray.length / 3;

    tVertexBitangentBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, tVertexBitangentBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(bitangentArray), gl.STATIC_DRAW);
    tVertexBitangentBuffer.itemSize = 3;
    tVertexBitangentBuffer.numItems = bitangentArray.length / 3;
}

/**
 * get texture coordinate for sphere.
 */
function getTextureCoord() {
    for (var i = 0; i < vertexArray.length; i += 3) {
        var tan2 = Math.atan2(vertexArray[i], vertexArray[i+2]) / Math.PI;
        if (tan2 >= 0.0) {
            tan2 = tan2 / 0.5;
        } else {
            tan2 = 1.0 + tan2 / 0.5;
        }
        textureCoordArray.push(tan2);
        textureCoordArray.push(Math.asin(vertexArray[i+1]) / Math.PI + 0.5);
    }
}

/**
 * find tangent space vectors.
 */
function getTangentSpace() {
    getTangentArray();
    getBitangentArray();
}

/**
 * get tangent array.
 */
function getTangentArray() {
    for (var i = 0; i < normalArray.length; i += 3) {
        if (normalArray[i+2] > 0.0) {
            tangentArray.push(1.0);
            tangentArray.push(0.0);
            tangentArray.push(-normalArray[i] / normalArray[i+2]);
        } else if (normalArray[i+2] < 0.0) {
            tangentArray.push(-1.0);
            tangentArray.push(0.0);
            tangentArray.push(normalArray[i] / normalArray[i+2]);
        } else {
            if (normalArray[i] > 0.0) {
                tangentArray.push(0.0);
                tangentArray.push(0.0);
                tangentArray.push(-1.0);
            } else {
                tangentArray.push(0.0);
                tangentArray.push(0.0);
                tangentArray.push(1.0);
            }
        }
    }
}

/**
 * get bitangent arrays.
 */
function getBitangentArray() {
    for (var i = 0; i < normalArray.length; i+=3) {
        var b = vec3.create();
        var n = vec3.fromValues(normalArray[i], normalArray[i+1], normalArray[i+2]);
        var t = vec3.fromValues(tangentArray[i], tangentArray[i+1], tangentArray[i+2]);
        vec3.cross(b, n, t);
        // vec3.cross(b, t, n);
        bitangentArray.push(b[0]);
        bitangentArray.push(b[1]);
        bitangentArray.push(b[2]);
    }
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

    gl.bindBuffer(gl.ARRAY_BUFFER, tVertexTangentBuffer);
    gl.vertexAttribPointer(shaderProgram.vertexTangentAttribute, tVertexTangentBuffer.itemSize, gl.FLOAT, false, 0, 0);

    gl.bindBuffer(gl.ARRAY_BUFFER, tVertexBitangentBuffer);
    gl.vertexAttribPointer(shaderProgram.vertexBitangentAttribute, tVertexBitangentBuffer.itemSize, gl.FLOAT, false, 0, 0);

    setMatrixUniforms();
    gl.drawArrays(gl.TRIANGLES, 0, tVertexPositionBuffer.numItems);
}

/**
 * Draw call that applies matrix transformations to cube
 */
function draw() {
    var lightPosEye4 = vec4.fromValues(25.0, 0.0, 30.0, 1.0);
    lightPosEye4 = vec4.transformMat4(lightPosEye4, lightPosEye4, mvMatrix);
    //console.log(vec4.str(lightPosEye4))
    var lightPosEye = vec3.fromValues(lightPosEye4[0], lightPosEye4[1], lightPosEye4[2]);

    // set up material parameters.
    var ka = vec4.fromValues(0.0, 0.0, 0.0, 1.0);
    var kd = vec4.fromValues(1.0, 1.0, 1.0, 1.0);
    var ks = vec4.fromValues(1.0, 1.0, 1.0, 1.0);

    // Set up light parameters
    var Ia = vec4.fromValues(1.0, 1.0, 0.8, 1.0);
    var Id = vec4.fromValues(1.0, 1.0, 0.8, 1.0);
    var Is = vec4.fromValues(1.0, 1.0, 0.8, 1.0);

    var transformVec = vec3.create();
    gl.viewport(0, 0, gl.viewportWidth, gl.viewportHeight);
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    mat4.perspective(pMatrix, 45, gl.viewportWidth / gl.viewportHeight, 0.1, 200.0);

    vec3.add(viewPt, eyePt, viewDir);
    mat4.lookAt(mvMatrix, eyePt, viewPt, up);

    keyListener();

    mvPushMatrix();
    mat4.translate(mvMatrix, mvMatrix, [-0.25, -1, 10]);
    mat4.rotateX(mvMatrix, mvMatrix, modelXRotationRadians);
    mat4.rotateY(mvMatrix, mvMatrix, modelYRotationRadians);
    setMatrixUniforms();
    drawSphere();
    uploadLightsToShader(lightPosEye, Ia, Id, Is);
    uploadMaterialToShader(ka, kd, ks);
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
