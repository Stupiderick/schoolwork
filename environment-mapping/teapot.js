
var gl;
var canvas;
var texID;
var useReflection;
var moveTeapot;

var shaderProgram;
var shaderTeapotProgram;

// Create a place to store the texture coords for the mesh
var cubeTCoordBuffer;

// Create a place to store terrain geometry
var cubeVertexBuffer;

// Create a place to store the triangles
var cubeTriIndexBuffer;

// Create ModelView matrix
var mvMatrix = mat4.create();

//Create Projection matrix
var pMatrix = mat4.create();

var nMatrix = mat3.create();

var mvMatrixStack = [];

var tVertexPositionBuffer;

//Create a place to store normals for shading
var tVertexNormalBuffer;

// Create a place to store the terrain triangles
var tIndexTriBuffer;

//Create a place to store the traingle edges
var tIndexEdgeBuffer;

// create a place to store the colors.
var tVertexColorBuffer;

// Create a place to store the texture
var cubeImage;
var cubeTexture;

// For animation
var then = 0;
var modelYRotationRadians = degToRad(0);
var teapotYRotationRadians = degToRad(0);
var lightYRotationRadians = degToRad(0);

// My arrays:
var vertexArray = [];
var faceArray = [];
var normalArray = [];
var meshNormalArray = [];
var colorArray = [];

// State the points
var eyePt = vec3.fromValues(0.0, 5.0, 14.0);
var viewDir = vec3.fromValues(0.0, -0.3, -1.0);
var up = vec3.fromValues(0.0, 1.0, 0.0);
var viewPt = vec3.fromValues(0.0, 0.0, 0.0);
var lightDir = vec3.fromValues(10.0, 10.0, 10.0);
var lightDirTemp = vec3.create();


var counting = 0;

/**
 * Sends Modelview matrix to shader
 */
function uploadModelViewMatrixToShader() {
    gl.useProgram(shaderTeapotProgram);
    gl.uniformMatrix4fv(shaderTeapotProgram.mvMatrixUniform, false, mvMatrix);
    gl.useProgram(shaderProgram);
    gl.uniformMatrix4fv(shaderProgram.mvMatrixUniform, false, mvMatrix);
}

/**
 * Sends projection matrix to shader
 */
function uploadProjectionMatrixToShader() {
    gl.useProgram(shaderTeapotProgram);
    gl.uniformMatrix4fv(shaderTeapotProgram.pMatrixUniform, false, pMatrix);
    gl.useProgram(shaderProgram);
    gl.uniformMatrix4fv(shaderProgram.pMatrixUniform, false, pMatrix);
}

/**
 * Generates and sends the normal matrix to the shader
 */
function uploadNormalMatrixToShader() {
    gl.useProgram(shaderTeapotProgram);
    mat3.fromMat4(nMatrix,mvMatrix);
    mat3.transpose(nMatrix,nMatrix);
    mat3.invert(nMatrix,nMatrix);
    gl.uniformMatrix3fv(shaderTeapotProgram.nMatrixUniform, false, nMatrix);
}

/**
 * Sends light information to the shader
 * @param {Float32Array} loc Location of light source
 * @param {Float32Array} a Ambient light strength
 * @param {Float32Array} d Diffuse light strength
 * @param {Float32Array} s Specular light strength
 */
function uploadLightsToShader(loc,a,d,s) {
  gl.useProgram(shaderTeapotProgram);
  gl.uniform3fv(shaderTeapotProgram.uniformLightPositionLoc, loc);
  gl.uniform3fv(shaderTeapotProgram.uniformAmbientLightColorLoc, a);
  gl.uniform3fv(shaderTeapotProgram.uniformDiffuseLightColorLoc, d);
  gl.uniform3fv(shaderTeapotProgram.uniformSpecularLightColorLoc, s);

}

function reflectionRadio(useReflection) {
    gl.uniform1f(shaderTeapotProgram.uniformUseReflection, useReflection);
}

function moveRadio(moveTeapot) {
    gl.uniform1f(shaderTeapotProgram.uniformMoveTeapot, moveTeapot);
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
 * Translates degrees to radians
 * @param {Number} degrees Degree input to function
 * @return {Number} The radians that correspond to the degree input
 */
function degToRad(degrees) {
    return degrees * Math.PI / 180;
}

/**
 * Translates radians to degrees
 * @param {Number} radian Radian input to function
 * @return {Number} The degrees that correspond to the radians input
 */
function radToDeg(radians) {
    return 180 * radians / Math.PI;
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
function setupCubeShaders() {
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
    console.log("Vertex attrib: ", shaderProgram.vertexPositionAttribute);
    gl.enableVertexAttribArray(shaderProgram.vertexPositionAttribute);

    shaderProgram.mvMatrixUniform = gl.getUniformLocation(shaderProgram, "uMVMatrix");
    shaderProgram.pMatrixUniform = gl.getUniformLocation(shaderProgram, "uPMatrix");
}

/**
 * Setup the fragment and vertex shaders for teapot
 */
function setupTeapotShaders() {
    vertexTeapotShader = loadShaderFromDOM("shader-teapot-vs");
    fragmentTeapotShader = loadShaderFromDOM("shader-teapot-fs");

    shaderTeapotProgram = gl.createProgram();
    gl.attachShader(shaderTeapotProgram, vertexTeapotShader);
    gl.attachShader(shaderTeapotProgram, fragmentTeapotShader);
    gl.linkProgram(shaderTeapotProgram);

    if (!gl.getProgramParameter(shaderTeapotProgram, gl.LINK_STATUS)) {
        alert("Failed to setup teapot shaders");
    }

    gl.useProgram(shaderTeapotProgram);

    shaderTeapotProgram.vertexPositionAttribute = gl.getAttribLocation(shaderTeapotProgram, "aVertexPosition");
    gl.enableVertexAttribArray(shaderTeapotProgram.vertexPositionAttribute);

    shaderTeapotProgram.vertexColorAttribute = gl.getAttribLocation(shaderTeapotProgram, "aVertexColor");
    gl.enableVertexAttribArray(shaderTeapotProgram.vertexColorAttribute);

    shaderTeapotProgram.vertexNormalAttribute = gl.getAttribLocation(shaderTeapotProgram, "aVertexNormal");
    gl.enableVertexAttribArray(shaderTeapotProgram.vertexNormalAttribute);

    shaderTeapotProgram.mvMatrixUniform = gl.getUniformLocation(shaderTeapotProgram, "uMVMatrix");
    shaderTeapotProgram.pMatrixUniform = gl.getUniformLocation(shaderTeapotProgram, "uPMatrix");
    shaderTeapotProgram.nMatrixUniform = gl.getUniformLocation(shaderTeapotProgram, "uNMatrix");
    shaderTeapotProgram.uniformLightPositionLoc = gl.getUniformLocation(shaderTeapotProgram, "uLightPosition");
    shaderTeapotProgram.uniformAmbientLightColorLoc = gl.getUniformLocation(shaderTeapotProgram, "uAmbientLightColor");
    shaderTeapotProgram.uniformDiffuseLightColorLoc = gl.getUniformLocation(shaderTeapotProgram, "uDiffuseLightColor");
    shaderTeapotProgram.uniformSpecularLightColorLoc = gl.getUniformLocation(shaderTeapotProgram, "uSpecularLightColor");
    shaderTeapotProgram.uniformDiffuseMaterialColor = gl.getUniformLocation(shaderTeapotProgram, "uDiffuseMaterialColor");
    shaderTeapotProgram.uniformAmbientMaterialColor = gl.getUniformLocation(shaderTeapotProgram, "uAmbientMaterialColor");
    shaderTeapotProgram.uniformSpecularMaterialColor = gl.getUniformLocation(shaderTeapotProgram, "uSpecularMaterialColor");
    shaderTeapotProgram.uniformUseReflection = gl.getUniformLocation(shaderTeapotProgram, "useReflection");
    shaderTeapotProgram.uniformMoveTeapot = gl.getUniformLocation(shaderTeapotProgram, "moveTeapot");
}

/**
 * Draw call that applies matrix transformations to cube
 */
function draw() {
    // get the status of whether the reflection is used.
    if (document.getElementById("useReflection").checked) {
        useReflection = 1.0;
    } else if (document.getElementById("noUseReflection").checked){
        useReflection = 0.0;
    }

    if (document.getElementById("moveTeapot").checked) {
        moveTeapot = 1.0;
    } else if (document.getElementById("moveCam").checked){
        moveTeapot = 0.0;
    }

    var transformVec = vec3.create();

    gl.viewport(0, 0, gl.viewportWidth, gl.viewportHeight);
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    // We'll use perspective
    mat4.perspective(pMatrix, degToRad(45), gl.viewportWidth / gl.viewportHeight, 0.1, 200.0);

    vec3.add(viewPt, eyePt, viewDir);
    // Then generate the lookat matrix and initialize the MV matrix to that view
    mat4.lookAt(mvMatrix, eyePt, viewPt, up);


    mvPushMatrix();
    vec3.set(transformVec, 0.0, 0.0, 0.0);
    mat4.translate(mvMatrix, mvMatrix, transformVec);



    if (moveTeapot == 1.0) {
        lightDirTemp = vec3.fromValues(10.0,10.0,10.0);
        //console.log(lightDirTemp)
        vec3.rotateY(lightDirTemp, lightDirTemp, [0.0,10.0,0.0], lightYRotationRadians);
    }
    else if (moveTeapot == 0.0) {
        //console.log("y")
        if (lightDirTemp != lightDir) {
            lightDir = lightDirTemp;
        }
    }


    mvPushMatrix();
    mat4.rotateY(mvMatrix, mvMatrix, teapotYRotationRadians);
    setMatrixUniforms();
    gl.useProgram(shaderTeapotProgram);
    reflectionRadio(useReflection);
    moveRadio(moveTeapot);

    drawTeapot();
    mvPopMatrix();

    mat4.lookAt(mvMatrix, eyePt, viewPt, up);
    mat4.rotateY(mvMatrix, mvMatrix, modelYRotationRadians);

    //vec3.rotateY(lightDir, lightDir, [0.0,10.0,0.0], 0);
    //lightDir = moveLight(lightDir);

    // var lightMat = mat4.fromValues(lightDir[0],0,0,0,0,lightDir[1],0,0,0,0,lightDir[2],0,0,0,0,1);
    // mat4.rotateY(lightMat, lightMat, modelYRotationRadians);
    // lightDir = vec3.fromValues(lightMat[0], lightMat[5], lightMat[10]);

    uploadLightsToShader(lightDir, [0.4,0.4,0.4], [1.0,1.0,1.0], [1.0,1.0,1.0]);
    gl.useProgram(shaderProgram);
    drawCube();
    mvPopMatrix();
}

/**
 * Draw a cube based on buffers.
 */
function drawCube(){
    // Draw the cube by binding the array buffer to the cube's vertices
    // array, setting attributes, and pushing it to GL.
    gl.useProgram(shaderProgram);
    gl.bindBuffer(gl.ARRAY_BUFFER, cubeVertexBuffer);
    gl.vertexAttribPointer(shaderProgram.vertexPositionAttribute, 3, gl.FLOAT, false, 0, 0);

    gl.uniform1i(gl.getUniformLocation(shaderProgram, "skyboxing"), 0);

    // Draw the cube.
    setMatrixUniforms();
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, cubeTriIndexBuffer);
    gl.drawElements(gl.TRIANGLES, 36, gl.UNSIGNED_SHORT, 0);
}

function drawTeapot() {
    //gl.polygonOffset(0,0);
    gl.useProgram(shaderTeapotProgram);
    gl.bindBuffer(gl.ARRAY_BUFFER, tVertexPositionBuffer);
    gl.vertexAttribPointer(shaderTeapotProgram.vertexPositionAttribute,
                      tVertexPositionBuffer.itemSize, gl.FLOAT, false, 0, 0);
    // Bind color buffer.
    gl.bindBuffer(gl.ARRAY_BUFFER, tVertexColorBuffer);
    gl.vertexAttribPointer(shaderTeapotProgram.vertexColorAttribute,
                        tVertexColorBuffer.itemSize, gl.FLOAT, false, 0, 0);

    // Bind normal buffer
    gl.bindBuffer(gl.ARRAY_BUFFER, tVertexNormalBuffer);
    gl.vertexAttribPointer(shaderTeapotProgram.vertexNormalAttribute,
                        tVertexNormalBuffer.itemSize, gl.FLOAT, false, 0, 0);

    //Draw
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, tIndexTriBuffer);
    gl.drawElements(gl.TRIANGLES, tIndexTriBuffer.numItems, gl.UNSIGNED_SHORT, 0);
}

/**
 * Animation to be called from tick. Updates global rotation values.
 */
function animate() {
    var speed = 0.02;

    window.addEventListener("keydown", function (event) {
        if (event.defaultPrevented) {
            return; // Do nothing if the event was already processed
        }



        if (moveTeapot == 1.0) {
            switch (event.key) {
                // left rotation.
                case "ArrowLeft":
                teapotYRotationRadians += speed;
                lightYRotationRadians -= speed;
                break;

                // right rotation
                case "ArrowRight":
                teapotYRotationRadians -= speed;
                lightYRotationRadians += speed;
                break;

                default:
                return; // Quit when this doesn't handle the key event.
            }
        } else {
            switch (event.key) {
                // left rotation.
                case "ArrowLeft":
                modelYRotationRadians += speed;
                teapotYRotationRadians += speed;
                break;

                // right rotation
                case "ArrowRight":
                modelYRotationRadians -= speed;
                teapotYRotationRadians -= speed;
                break;

                default:
                return; // Quit when this doesn't handle the key event.
            }
        }
        console.log(lightDirTemp, lightDir)

        // Cancel the default action to avoid it being handled twice
        event.preventDefault();
    }, true);
}

/**
 * Creates texture for application to cube.
 */
function setupTextures() {
    var counting = 0;
    var img = new Array(6);
    var urls = [
       "images/pos-x.png", "images/neg-x.png",
       "images/pos-y.png", "images/neg-y.png",
       "images/pos-z.png", "images/neg-z.png"
    ];
    for (var i = 0; i < 6; i++) {
        img[i] = new Image();
        img[i].onload = function() {
            counting++;
            if (counting == 6) {
                texID = gl.createTexture();
                gl.bindTexture(gl.TEXTURE_CUBE_MAP, texID);
                var targets = [
                    gl.TEXTURE_CUBE_MAP_POSITIVE_X, gl.TEXTURE_CUBE_MAP_NEGATIVE_X,
                    gl.TEXTURE_CUBE_MAP_POSITIVE_Y, gl.TEXTURE_CUBE_MAP_NEGATIVE_Y,
                    gl.TEXTURE_CUBE_MAP_POSITIVE_Z, gl.TEXTURE_CUBE_MAP_NEGATIVE_Z
                ];

                for (var j = 0; j < 6; j++) {
                    gl.texParameteri(gl.TEXTURE_CUBE_MAP, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
                    gl.texParameteri(gl.TEXTURE_CUBE_MAP, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
                    gl.texImage2D(targets[j], 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, img[j]);
                }
                gl.generateMipmap(gl.TEXTURE_CUBE_MAP);
            }
        }
        img[i].src = urls[i];
    }
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
function handleTextureLoaded(image, texture) {
    console.log("handleTextureLoaded, image = " + image);
    gl.bindTexture(gl.TEXTURE_2D, texture);
    gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA,gl.UNSIGNED_BYTE, image);

    // Check if the image is a power of 2 in both dimensions.
    if (isPowerOf2(image.width) && isPowerOf2(image.height)) {
        // Yes, it's a power of 2. Generate mips.
        gl.generateMipmap(gl.TEXTURE_2D);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR_MIPMAP_NEAREST);
        console.log("Loaded power of 2 texture");
    } else {
        // No, it's not a power of 2. Turn of mips and set wrapping to clamp to edge
        gl.texParameteri(gl.TETXURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
        gl.texParameteri(gl.TETXURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
        gl.texParameteri(gl.TETXURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
        console.log("Loaded non-power of 2 texture");
    }
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
}


/**
 * Sets up buffers for cube.
 */
function setupBuffers() {
    readTextFile("teapot_0.obj", parseObj);
}

/**
 * Parse the .obj file to get the vertex and face arrays.
 * @param {string} streamOfFile the string of the .obj contents.
 */
function parseObj(streamOfFile) {
    var coords = streamOfFile.split("\n");
    //console.log(coords.length);

    for (var i = 0; i < coords.length; i++) {
        var pointData = coords[i].split(" ");
        if (pointData[0] == "v") {
            vertexArray.push(parseFloat(pointData[1]));
            vertexArray.push(parseFloat(pointData[2]));
            vertexArray.push(parseFloat(pointData[3]));
        } else if (pointData[0] == "f") {
            faceArray.push(parseFloat(pointData[2]) - 1);
            faceArray.push(parseFloat(pointData[3]) - 1);
            faceArray.push(parseFloat(pointData[4]) - 1);
        } else {
            console.log(i, pointData);
        }
    }

    for (var i = 0; i < 3606; i++) {
        normalArray.push(0.0);
    }

    for (var i = 0; i < 1202; i++) {
        colorArray.push(0.0,1.0,1.0,1.0);
    }

    getNormalArray();

    gl.useProgram(shaderTeapotProgram);
    setupTeapotShaders();
    setupTeapotBuffers();

    gl.useProgram(shaderProgram);
    setupCubeShaders();
    setupCubeBuffers();
    setupTextures();

    tick();
}


/**
 * Get the normal array from the parsed vertex and face array.
 */
function getNormalArray() {
    for (var i = 0; i < faceArray.length / 3; i++) {
        // get the points info of the meshes.
        var pos0 = faceArray[i*3];
        var pos1 = faceArray[i*3+1];
        var pos2 = faceArray[i*3+2];

        var p0x = vertexArray[pos0*3];
        var p0y = vertexArray[pos0*3+1];
        var p0z = vertexArray[pos0*3+2];

        var p1x = vertexArray[pos1*3];
        var p1y = vertexArray[pos1*3+1];
        var p1z = vertexArray[pos1*3+2];

        var p2x = vertexArray[pos2*3];
        var p2y = vertexArray[pos2*3+1];
        var p2z = vertexArray[pos2*3+2];

        // get two lines.
        var line10 = [p0x-p1x, p0y-p1y, p0z-p1z];
        var line20 = [p0x-p2x, p0y-p2y, p0z-p2z];

        var meshNormal = [];

        // get the mesh normal vertices by using gl-matrix function
        vec3.cross(meshNormal, line10, line20);
        vec3.normalize(meshNormal, meshNormal);

        for (var j = 0; j < 3; j++) {
            normalArray[(faceArray[i*3+j])*3] += meshNormal[0];
            normalArray[(faceArray[i*3+j])*3+1] += meshNormal[1];
            normalArray[(faceArray[i*3+j])*3+2] += meshNormal[2];
        }
    }

    // normalise the normalArray.
    for (var i = 0; i < normalArray.length; i=i+3) {
        aNormal = vec3.fromValues(normalArray[i], normalArray[i+1], normalArray[i+2]);
        vec3.normalize(aNormal, aNormal);
        normalArray[i] = aNormal[0];
        normalArray[i+1] = aNormal[1];
        normalArray[i+2] = aNormal[2];
    }
}

/**
 * Populate buffers with data
 */
function setupCubeBuffers() {
    // Create a buffer for the cube's vertices.
    cubeVertexBuffer = gl.createBuffer();

    // Select the cubeVerticesBuffer as the one to apply vertex
    // operations to from here out.
    gl.bindBuffer(gl.ARRAY_BUFFER, cubeVertexBuffer);

    // Now create an array of vertices for the cube.
    var vertices = [
        // Front face
        -20.0, -20.0,  20.0,
         20.0, -20.0,  20.0,
         20.0,  20.0,  20.0,
        -20.0,  20.0,  20.0,

        // Back face
        -20.0, -20.0, -20.0,
        -20.0,  20.0, -20.0,
         20.0,  20.0, -20.0,
         20.0, -20.0, -20.0,

        // Top face
        -20.0,  20.0, -20.0,
        -20.0,  20.0,  20.0,
         20.0,  20.0,  20.0,
         20.0,  20.0, -20.0,

        // Bottom face
        -20.0, -20.0, -20.0,
         20.0, -20.0, -20.0,
         20.0, -20.0,  20.0,
        -20.0, -20.0,  20.0,

        // Right face
         20.0, -20.0, -20.0,
         20.0,  20.0, -20.0,
         20.0,  20.0,  20.0,
         20.0, -20.0,  20.0,

        // Left face
        -20.0, -20.0, -20.0,
        -20.0, -20.0,  20.0,
        -20.0,  20.0,  20.0,
        -20.0,  20.0, -20.0
    ];

    // Now pass the list of vertices into WebGL to build the shape. We
    // do this by creating a Float32Array from the JavaScript array,
    // then use it to fill the current vertex buffer.
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);

    // Build the element array buffer; this specifies the indices
    // into the vertex array for each face's vertices.
    cubeTriIndexBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, cubeTriIndexBuffer);

    // This array defines each face as two triangles, using the
    // indices into the vertex array to specify each triangle's
    // position.
    var cubeVertexIndices = [
        0,  1,  2,      0,  2,  3,    // front
        4,  5,  6,      4,  6,  7,    // back
        8,  9,  10,     8,  10, 11,   // top
        12, 13, 14,     12, 14, 15,   // bottom
        16, 17, 18,     16, 18, 19,   // right
        20, 21, 22,     20, 22, 23    // left
    ];

    // Now send the element array to GL
    gl.bufferData(gl.ELEMENT_ARRAY_BUFFER,
                  new Uint16Array(cubeVertexIndices), gl.STATIC_DRAW);
}


/**
 * Set up the teapot buffer.
 */
function setupTeapotBuffers() {
    gl.useProgram(shaderTeapotProgram);
    tVertexPositionBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, tVertexPositionBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertexArray), gl.STATIC_DRAW);
    tVertexPositionBuffer.itemSize = 3;
    tVertexPositionBuffer.numItems = vertexArray.length / 3;


    tVertexColorBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, tVertexColorBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(colorArray), gl.STATIC_DRAW);
    tVertexColorBuffer.itemSize = 4;
    tVertexColorBuffer.numItems = colorArray.length / 4;

    // Specify normals to be able to do lighting calculations
    tVertexNormalBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, tVertexNormalBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(normalArray), gl.STATIC_DRAW);
    console.log("normal",normalArray)
    tVertexNormalBuffer.itemSize = 3;
    tVertexNormalBuffer.numItems = normalArray.length / 3;

    // Specify faces of the terrain
    tIndexTriBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, tIndexTriBuffer);
    gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint16Array(faceArray), gl.STATIC_DRAW);
    tIndexTriBuffer.itemSize = 1;
    tIndexTriBuffer.numItems = faceArray.length;
}


function requestCORSIfNotSameOrigin(img, url) {
    if ((new URL(url)).origin !== window.location.origin) {
        img.crossOrigin = "";
    }
}


/**
 * Startup function called from html code to start program.
 */
function startup() {
    canvas = document.getElementById("myGLCanvas");
    gl = createGLContext(canvas);
    gl.clearColor(0.0, 0.0, 0.0, 1.0);
    gl.enable(gl.DEPTH_TEST);
    setupBuffers();
}


/**
 * Tick called for every animation frame.
 */
function tick() {
    requestAnimFrame(tick);
    draw();
    animate();
}
