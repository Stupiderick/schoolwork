/**
 * Parts of the codes were grabbed from the course website which is permitted
 */

var gl;
var canvas;
var shaderProgram;
var vertexPositionBuffer;

// Create a place to store vertex colors
var vertexColorBuffer;
var mvMatrix = mat4.create();
var rotAngle = 0;
var lastTime = 0;


/**
 * Sends projection/modelview matrices to shader
 */
function setMatrixUniforms() {
    gl.uniformMatrix4fv(shaderProgram.mvMatrixUniform, false, mvMatrix);
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
    for (var i = 0; i < names.length; i++) {
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

    shaderProgram.vertexColorAttribute = gl.getAttribLocation(shaderProgram, "aVertexColor");
    gl.enableVertexAttribArray(shaderProgram.vertexColorAttribute);
    shaderProgram.mvMatrixUniform = gl.getUniformLocation(shaderProgram, "uMVMatrix");
}

/**
 * Populate buffers with data
 */
function setupBuffers() {
    vertexPositionBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vertexPositionBuffer);
    var triangleVertices = [
        /**
         * The upper part of the badge.
         */
        -0.7,   0.9,   0.0,
        -0.7,   0.65,  0.0,
         0.7,   0.9,   0.0,

        -0.7,   0.65,  0.0,
         0.7,   0.9,   0.0,
         0.7,   0.65,  0.0,

        -0.55,  0.65,  0.0,
        -0.25,  0.65,  0.0,
        -0.55, -0.25,  0.0,

        -0.25,  0.65,  0.0,
        -0.55, -0.25,  0.0,
        -0.25, -0.25,  0.0,

         0.55,  0.65,  0.0,
         0.25,  0.65,  0.0,
         0.55, -0.25,  0.0,

         0.25,  0.65,  0.0,
         0.55, -0.25,  0.0,
         0.25, -0.25,  0.0,

        -0.25,  0.4,   0.0,
        -0.15,  0.4,   0.0,
        -0.25,  0.0,   0.0,

        -0.15,  0.4,   0.0,
        -0.25,  0.0,   0.0,
        -0.15,  0.0,   0.0,

         0.25,  0.4,   0.0,
         0.15,  0.4,   0.0,
         0.25,  0.0,   0.0,

         0.15,  0.4,   0.0,
         0.25,  0.0,   0.0,
         0.15,  0.0,   0.0,

        /**
         * The left lower part of the badge.
         */
        -0.05, -0.3,   0.0,
        -0.15, -0.3,   0.0,
        -0.05, -0.765, 0.0,

        -0.15, -0.3,   0.0,
        -0.05, -0.765, 0.0,
        -0.15, -0.692, 0.0,

        -0.25, -0.3,   0.0,
        -0.35, -0.3,   0.0,
        -0.25, -0.619, 0.0,

        -0.35, -0.3,   0.0,
        -0.25, -0.619, 0.0,
        -0.35, -0.546, 0.0,

        -0.45, -0.3,   0.0,
        -0.55, -0.3,   0.0,
        -0.45, -0.473, 0.0,

        -0.55, -0.3,   0.0,
        -0.45, -0.473, 0.0,
        -0.55, -0.4,   0.0,

        /**
         * The right lower part of the badge.
         */
         0.05, -0.3,   0.0,
         0.15, -0.3,   0.0,
         0.05, -0.765, 0.0,

         0.15, -0.3,   0.0,
         0.05, -0.765, 0.0,
         0.15, -0.692, 0.0,

         0.25, -0.3,   0.0,
         0.35, -0.3,   0.0,
         0.25, -0.619, 0.0,

         0.35, -0.3,   0.0,
         0.25, -0.619, 0.0,
         0.35, -0.546, 0.0,

         0.45, -0.3,   0.0,
         0.55, -0.3,   0.0,
         0.45, -0.473, 0.0,

         0.55, -0.3,   0.0,
         0.45, -0.473, 0.0,
         0.55, -0.4,   0.0
     ];

     gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(triangleVertices), gl.STATIC_DRAW);
     vertexPositionBuffer.itemSize = 3;
     vertexPositionBuffer.numberOfItems = 66;

     vertexColorBuffer = gl.createBuffer();
     gl.bindBuffer(gl.ARRAY_BUFFER, vertexColorBuffer);
     var colors = [
         /**
          * Illini blue for the upper part.
          */
         0.09, 0.157, 0.286, 1.0,
         0.09, 0.157, 0.286, 1.0,
         0.09, 0.157, 0.286, 1.0,

         0.09, 0.157, 0.286, 1.0,
         0.09, 0.157, 0.286, 1.0,
         0.09, 0.157, 0.286, 1.0,

         0.09, 0.157, 0.286, 1.0,
         0.09, 0.157, 0.286, 1.0,
         0.09, 0.157, 0.286, 1.0,

         0.09, 0.157, 0.286, 1.0,
         0.09, 0.157, 0.286, 1.0,
         0.09, 0.157, 0.286, 1.0,

         0.09, 0.157, 0.286, 1.0,
         0.09, 0.157, 0.286, 1.0,
         0.09, 0.157, 0.286, 1.0,

         0.09, 0.157, 0.286, 1.0,
         0.09, 0.157, 0.286, 1.0,
         0.09, 0.157, 0.286, 1.0,

         0.09, 0.157, 0.286, 1.0,
         0.09, 0.157, 0.286, 1.0,
         0.09, 0.157, 0.286, 1.0,

         0.09, 0.157, 0.286, 1.0,
         0.09, 0.157, 0.286, 1.0,
         0.09, 0.157, 0.286, 1.0,

         0.09, 0.157, 0.286, 1.0,
         0.09, 0.157, 0.286, 1.0,
         0.09, 0.157, 0.286, 1.0,

         0.09, 0.157, 0.286, 1.0,
         0.09, 0.157, 0.286, 1.0,
         0.09, 0.157, 0.286, 1.0,

         /**
          * Illini orange for the lower part.
          */
         0.851, 0.337, 0.259, 1.0,
         0.851, 0.337, 0.259, 1.0,
         0.851, 0.337, 0.259, 1.0,

         0.851, 0.337, 0.259, 1.0,
         0.851, 0.337, 0.259, 1.0,
         0.851, 0.337, 0.259, 1.0,

         0.851, 0.337, 0.259, 1.0,
         0.851, 0.337, 0.259, 1.0,
         0.851, 0.337, 0.259, 1.0,

         0.851, 0.337, 0.259, 1.0,
         0.851, 0.337, 0.259, 1.0,
         0.851, 0.337, 0.259, 1.0,

         0.851, 0.337, 0.259, 1.0,
         0.851, 0.337, 0.259, 1.0,
         0.851, 0.337, 0.259, 1.0,

         0.851, 0.337, 0.259, 1.0,
         0.851, 0.337, 0.259, 1.0,
         0.851, 0.337, 0.259, 1.0,

         0.851, 0.337, 0.259, 1.0,
         0.851, 0.337, 0.259, 1.0,
         0.851, 0.337, 0.259, 1.0,

         0.851, 0.337, 0.259, 1.0,
         0.851, 0.337, 0.259, 1.0,
         0.851, 0.337, 0.259, 1.0,

         0.851, 0.337, 0.259, 1.0,
         0.851, 0.337, 0.259, 1.0,
         0.851, 0.337, 0.259, 1.0,

         0.851, 0.337, 0.259, 1.0,
         0.851, 0.337, 0.259, 1.0,
         0.851, 0.337, 0.259, 1.0,

         0.851, 0.337, 0.259, 1.0,
         0.851, 0.337, 0.259, 1.0,
         0.851, 0.337, 0.259, 1.0,

         0.851, 0.337, 0.259, 1.0,
         0.851, 0.337, 0.259, 1.0,
         0.851, 0.337, 0.259, 1.0
     ];

     gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(colors), gl.STATIC_DRAW);
     vertexColorBuffer.itemSize = 4;
     vertexColorBuffer.numItems = 66;
}

/**
 * Draw call that applies matrix transformations to model and draws model in frame
 * Rotate along the y-axis!
 */
function draw() {
    gl.viewport(0, 0, gl.viewportWidth, gl.viewportHeight);
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
    mat4.identity(mvMatrix);
    mat4.rotateY(mvMatrix, mvMatrix, degToRad(rotAngle));
    gl.bindBuffer(gl.ARRAY_BUFFER, vertexPositionBuffer);
    gl.vertexAttribPointer(shaderProgram.vertexPositionAttribute,
                         vertexPositionBuffer.itemSize, gl.FLOAT, false, 0, 0);
    gl.bindBuffer(gl.ARRAY_BUFFER, vertexColorBuffer);
    gl.vertexAttribPointer(shaderProgram.vertexColorAttribute,
                            vertexColorBuffer.itemSize, gl.FLOAT, false, 0, 0);

    setMatrixUniforms();
    gl.drawArrays(gl.TRIANGLES, 0, vertexPositionBuffer.numberOfItems);
}

/**
 * Animation to be called from tick. Updates globals and performs animation for each tick.
 */
 var scalar = 0;
 function animate() {
     /**
      * Uniform animation (rotation) using time as seed.
      */
     var timeNow = new Date().getTime();
     if (lastTime != 0) {
         var elapsed = timeNow - lastTime;
         rotAngle = (rotAngle + 0.75) % 360;
     }
     lastTime = timeNow;

     scalar += 0.1;
     gl.bindBuffer(gl.ARRAY_BUFFER, vertexPositionBuffer);

     var triangleVertices = [
         /**
          * Without non-uniform animation.
          */
         -0.7,   0.9,   0.0,
         -0.7,   0.65,  0.0,
          0.7,   0.9,   0.0,

         -0.7,   0.65,  0.0,
          0.7,   0.9,   0.0,
          0.7,   0.65,  0.0,

         -0.55,  0.65,  0.0,
         -0.25,  0.65,  0.0,
         -0.55, -0.25,  0.0,

         -0.25,  0.65,  0.0,
         -0.55, -0.25,  0.0,
         -0.25, -0.25,  0.0,

          0.55,  0.65,  0.0,
          0.25,  0.65,  0.0,
          0.55, -0.25,  0.0,

          0.25,  0.65,  0.0,
          0.55, -0.25,  0.0,
          0.25, -0.25,  0.0,

         -0.25,  0.4,   0.0,
         -0.15,  0.4,   0.0,
         -0.25,  0.0,   0.0,

         -0.15,  0.4,   0.0,
         -0.25,  0.0,   0.0,
         -0.15,  0.0,   0.0,

          0.25,  0.4,   0.0,
          0.15,  0.4,   0.0,
          0.25,  0.0,   0.0,

          0.15,  0.4,   0.0,
          0.25,  0.0,   0.0,
          0.15,  0.0,   0.0,

         /**
          * With the non-uniform animation. Using sine and cosine function.
          * Left part.
          */
         -0.05, -0.3,   0.0,
         -0.15, -0.3,   0.0,
         -0.05 + Math.sin(scalar - 0.50) * 0.5, -0.765 - Math.cos(scalar - 0.5) * 0.05, 0.0,

         -0.15, -0.3,   0.0,
         -0.05 + Math.sin(scalar - 0.50) * 0.5, -0.765 - Math.cos(scalar - 0.5) * 0.05, 0.0,
         -0.15 + Math.sin(scalar - 0.40) * 0.4, -0.692 - Math.cos(scalar - 0.4) * 0.04, 0.0,

         -0.25, -0.3,   0.0,
         -0.35, -0.3,   0.0,
         -0.25 + Math.sin(scalar - 0.30) * 0.3, -0.619 - Math.cos(scalar - 0.3) * 0.03, 0.0,

         -0.35, -0.3,   0.0,
         -0.25 + Math.sin(scalar - 0.30) * 0.3, -0.619 - Math.cos(scalar - 0.3) * 0.03, 0.0,
         -0.35 + Math.sin(scalar - 0.20) * 0.2, -0.546 - Math.cos(scalar - 0.2) * 0.02, 0.0,

         -0.45, -0.3,   0.0,
         -0.55, -0.3,   0.0,
         -0.45 + Math.sin(scalar - 0.10) * 0.1, -0.473 - Math.cos(scalar - 0.1) * 0.01, 0.0,

         -0.55, -0.3,   0.0,
         -0.45 + Math.sin(scalar - 0.10) * 0.1, -0.473 - Math.cos(scalar - 0.1) * 0.01, 0.0,
         -0.55 + Math.sin(scalar - 0.00) * 0.1, -0.400 - Math.cos(scalar - 0.0) * 0.00, 0.0,

         /**
          * Right part.
          */
         0.05, -0.3,   0.0,
         0.15, -0.3,   0.0,
         0.05 + Math.sin(scalar - 0.50) * 0.5, -0.765 - Math.cos(scalar - 0.5) * 0.05, 0.0,

         0.15, -0.3,   0.0,
         0.05 + Math.sin(scalar - 0.50) * 0.5, -0.765 - Math.cos(scalar - 0.5) * 0.05, 0.0,
         0.15 + Math.sin(scalar - 0.60) * 0.6, -0.692 - Math.cos(scalar - 0.6) * 0.06, 0.0,

         0.25, -0.3,   0.0,
         0.35, -0.3,   0.0,
         0.25 + Math.sin(scalar - 0.70) * 0.7, -0.619 - Math.cos(scalar - 0.7) * 0.07, 0.0,

         0.35, -0.3,   0.0,
         0.25 + Math.sin(scalar - 0.70) * 0.7, -0.619 - Math.cos(scalar - 0.7) * 0.07, 0.0,
         0.35 + Math.sin(scalar - 0.80) * 0.8, -0.546 - Math.cos(scalar - 0.8) * 0.08, 0.0,

         0.45, -0.3,   0.0,
         0.55, -0.3,   0.0,
         0.45 + Math.sin(scalar - 0.90) * 0.9, -0.473 - Math.cos(scalar - 0.9) * 0.09, 0.0,

         0.55, -0.3,   0.0,
         0.45 + Math.sin(scalar - 0.90) * 0.9, -0.473 - Math.cos(scalar - 0.9) * 0.09, 0.0,
         0.55 + Math.sin(scalar - 1.00) * 1.0, -0.400 - Math.cos(scalar - 1.0) * 0.10, 0.0
     ];

     gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(triangleVertices), gl.STATIC_DRAW);
     vertexPositionBuffer.itemSize = 3;
     vertexPositionBuffer.numberOfItems = 66;
 }

/**
 * Startup function called from html code to start program.
 */
function startup() {
    canvas = document.getElementById("myGLCanvas");
    gl = createGLContext(canvas);
    setupShaders();
    setupBuffers();
    gl.clearColor(1.0, 1.0, 1.0, 1.0);
    gl.enable(gl.DEPTH_TEST);
    tick();
}

/**
 * Tick called for every animation frame.
 */
function tick() {
    requestAnimFrame(tick);
    draw();
    animate();
}
