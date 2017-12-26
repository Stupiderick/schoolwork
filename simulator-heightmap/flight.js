
var gl;
var canvas;
var shaderProgram;
var vertexPositionBuffer;
var days = 0;
var showFog;
// Create a place to store terrain geometry
var tVertexPositionBuffer;

//Create a place to store normals for shading
var tVertexNormalBuffer;

// Create a place to store the terrain triangles
var tIndexTriBuffer;

//Create a place to store the traingle edges
var tIndexEdgeBuffer;

// create a place to store the colors.
var tVertexColorBuffer;

// store the height information of the terrain
var heightData = [];

// store the color information of the terrain
var colorData = [];

// View parameters
var eyePt = vec3.fromValues(0.0, 0.0, -1.0);
var viewDir = vec3.fromValues(0.0, 0.0, -1.0);
var up = vec3.fromValues(0.0, 1.0, 0.0);
var viewPt = vec3.fromValues(0.0, 0.0, 0.0);
var speedFactor = vec3.create();
var speed = 0.002;
//var curViewDir = new glMatrix.ARRAY_TYPE(4);
var curViewDir = quat.fromValues(viewDir[0], viewDir[1], viewDir[2], 0.0);
//var curUp = new glMatrix.ARRAY_TYPE(4);
var curUp = quat.fromValues(up[0], up[1], up[2], 0.0);
// temp quaternion
var tempQuat = quat.create();
var normViewDir = vec3.create();
var invQuat = quat.create();

// Create the normal
var nMatrix = mat3.create();

// Create ModelView matrix
var mvMatrix = mat4.create();

//Create Projection matrix
var pMatrix = mat4.create();

var mvMatrixStack = [];
// record angle that has spinned.
var angle = 0.0;

//-------------------------------------------------------------------------
/**
 * Populates terrain buffers for terrain generation
 */
function setupBuffers() {
    var vTerrain = [];
    var fTerrain = [];
    var nTerrain = [];
    var eTerrain = [];
    var cTerrain = [];
    var gridN = 1024;

    var numT = terrainReader(gridN, -1, 1, -1, 1, vTerrain, fTerrain, nTerrain);
    colorReader(gridN, -1, 1, -1, 1, cTerrain);

    console.log("Generated ", numT, " triangles");
    tVertexPositionBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, tVertexPositionBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vTerrain), gl.STATIC_DRAW);
    tVertexPositionBuffer.itemSize = 3;
    tVertexPositionBuffer.numItems = (gridN + 1) * (gridN + 1);

    tVertexColorBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, tVertexColorBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(cTerrain), gl.STATIC_DRAW);
    tVertexColorBuffer.itemSize = 3;
    tVertexColorBuffer.numItems = (gridN + 1) * (gridN + 1);

    // Specify normals to be able to do lighting calculations
    tVertexNormalBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, tVertexNormalBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(nTerrain), gl.STATIC_DRAW);
    tVertexNormalBuffer.itemSize = 3;
    tVertexNormalBuffer.numItems = (gridN + 1) * (gridN + 1);

    // Specify faces of the terrain
    tIndexTriBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, tIndexTriBuffer);
    gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint32Array(fTerrain),
                  gl.STATIC_DRAW);
    tIndexTriBuffer.itemSize = 1;
    tIndexTriBuffer.numItems = fTerrain.length;
}

//-------------------------------------------------------------------------
/**
 * Draws terrain from populated buffers
 */
function drawTerrain(){
    //gl.polygonOffset(0,0);
    gl.bindBuffer(gl.ARRAY_BUFFER, tVertexPositionBuffer);
    gl.vertexAttribPointer(shaderProgram.vertexPositionAttribute,
                      tVertexPositionBuffer.itemSize, gl.FLOAT, false, 0, 0);

    gl.bindBuffer(gl.ARRAY_BUFFER, tVertexColorBuffer);
    gl.vertexAttribPointer(shaderProgram.vertexColorAttribute,
                        tVertexColorBuffer.itemSize, gl.FLOAT, false, 0, 0);
 // Bind normal buffer
    gl.bindBuffer(gl.ARRAY_BUFFER, tVertexNormalBuffer);
    gl.vertexAttribPointer(shaderProgram.vertexNormalAttribute,
                           tVertexNormalBuffer.itemSize, gl.FLOAT, false, 0, 0);

    //Draw
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, tIndexTriBuffer);
    // setMatrixUniforms();
    var ext = gl.getExtension('OES_element_index_uint');
    gl.drawElements(gl.TRIANGLES, tIndexTriBuffer.numItems, gl.UNSIGNED_INT, 0);
}

//-------------------------------------------------------------------------
/**
 * Sends Modelview matrix to shader
 */
function uploadModelViewMatrixToShader() {
    gl.uniformMatrix4fv(shaderProgram.mvMatrixUniform, false, mvMatrix);
}

//-------------------------------------------------------------------------
/**
 * Sends projection matrix to shader
 */
function uploadProjectionMatrixToShader() {
    gl.uniformMatrix4fv(shaderProgram.pMatrixUniform, false, pMatrix);
}

//----------------------------------------------------------------------------------
/**
 * Sends projection/modelview matrices to shader
 */
function setMatrixUniforms() {
    uploadModelViewMatrixToShader();
    uploadNormalMatrixToShader();
    uploadProjectionMatrixToShader();
}

//-------------------------------------------------------------------------
/**
 * Generates and sends the normal matrix to the shader
 */
function uploadNormalMatrixToShader() {
    mat3.fromMat4(nMatrix, mvMatrix);
    mat3.transpose(nMatrix, nMatrix);
    mat3.invert(nMatrix, nMatrix);
    gl.uniformMatrix3fv(shaderProgram.nMatrixUniform, false, nMatrix);
}

function fogRadio(showFog) {
    gl.uniform1f(shaderProgram.uniformShowFog, showFog);
}

//----------------------------------------------------------------------------------
/**
 * Pushes matrix onto modelview matrix stack
 */
function mvPushMatrix() {
    var copy = mat4.clone(mvMatrix);
    mvMatrixStack.push(copy);
}


//----------------------------------------------------------------------------------
/**
 * Pops matrix off of modelview matrix stack
 */
function mvPopMatrix() {
    if (mvMatrixStack.length == 0) {
        throw "Invalid popMatrix!";
    }
    mvMatrix = mvMatrixStack.pop();
}


//----------------------------------------------------------------------------------
/**
 * Translates degrees to radians
 * @param {Number} degrees Degree input to function
 * @return {Number} The radians that correspond to the degree input
 */
function degToRad(degrees) {
    return degrees * Math.PI / 180;
}

//----------------------------------------------------------------------------------
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

//----------------------------------------------------------------------------------
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


//----------------------------------------------------------------------------------
/**
 * Setup the fragment and vertex shaders
 */
function setupShaders(vshader, fshader) {
    vertexShader = loadShaderFromDOM(vshader);
    fragmentShader = loadShaderFromDOM(fshader);

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

    shaderProgram.vertexNormalAttribute = gl.getAttribLocation(shaderProgram, "aVertexNormal");
    gl.enableVertexAttribArray(shaderProgram.vertexNormalAttribute);

    shaderProgram.mvMatrixUniform = gl.getUniformLocation(shaderProgram, "uMVMatrix");
    shaderProgram.pMatrixUniform = gl.getUniformLocation(shaderProgram, "uPMatrix");
    shaderProgram.nMatrixUniform = gl.getUniformLocation(shaderProgram, "uNMatrix");
    shaderProgram.uniformShowFog = gl.getUniformLocation(shaderProgram, "showFog");
}


//----------------------------------------------------------------------------------
/**
 * Draw call that applies matrix transformations to model and draws model in frame
 */
function draw() {
    if (document.getElementById("phong-phong-fog").checked) {
        showFog = 1.0;
    } else if (document.getElementById("phong-phong-no-fog").checked){
        showFog = 0.0;
    }

    var transformVec = vec3.create();

    gl.viewport(0, 0, gl.viewportWidth, gl.viewportHeight);
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    // We'll use perspective
    mat4.perspective(pMatrix, degToRad(45), gl.viewportWidth / gl.viewportHeight, 0.1, 200.0);

    // We want to look down -z, so create a lookat point in that direction

    vec3.normalize(normViewDir, viewDir);

    speedFactor[0] = normViewDir[0] * speed;
    speedFactor[1] = normViewDir[1] * speed;
    speedFactor[2] = normViewDir[2] * speed;

    vec3.add(eyePt, speedFactor, eyePt);
    vec3.add(viewPt, eyePt, viewDir);

    // Then generate the lookat matrix and initialize the MV matrix to that view
    mat4.lookAt(mvMatrix, eyePt, viewPt, up);

    keyListener();

    document.getElementById("speedscreen").innerHTML = parseInt(speed * 1000 + 300);

    //Draw Terrain
    mvPushMatrix();
    vec3.set(transformVec,0.0,-0.25,-3.0);
    mat4.translate(mvMatrix, mvMatrix,transformVec);
    mat4.rotateX(mvMatrix, mvMatrix, degToRad(-75));
    mat4.rotateZ(mvMatrix, mvMatrix, degToRad(25));
    setMatrixUniforms();
    fogRadio(showFog);
    drawTerrain();
    mvPopMatrix();
}

/**
 * listen the key press to control the flight.
 */
function keyListener() {
    // Control pitch and roll.
    window.addEventListener("keydown", function (event) {
        if (event.defaultPrevented) {
            return; // Do nothing if the event was already processed
        }

        switch (event.key) {
            // speed down ------------------------------
            case "-":
            speed > 0.001 ? speed -= 0.001 : speed = 0.001;
            break;

            // Prevent distinguishing
            case "_":
            speed > 0.001 ? speed -= 0.001 : speed = 0.001;
            break;
            //------------------------------------------

            // speed up --------------------------------
            case "+":
            speed += 0.001;
            break;

            // Prevent distinguishing
            case "=":
            speed += 0.001;
            break;
            //------------------------------------------

            // Roll countercolockwisely
            case "ArrowLeft":
            quat.fromEuler(tempQuat, 0.0, 0.0, 1.0);
            angle += (Math.PI / 180.0);
            quat.invert(invQuat, tempQuat);
            quat.multiply(curUp, tempQuat, quat.multiply(curUp, curUp, invQuat));
            up = vec3.fromValues(curUp[0], curUp[1], curUp[2]);
            break;

            // Roll clockwisely
            case "ArrowRight":
            quat.fromEuler(tempQuat, 0.0, 0.0, -1.0);
            angle -= (Math.PI / 180.0);
            quat.invert(invQuat, tempQuat);
            quat.multiply(curUp, tempQuat, quat.multiply(curUp, curUp, invQuat));
            up = vec3.fromValues(curUp[0], curUp[1], curUp[2]);
            break;

            // Pitch up
            case "ArrowUp":
            quat.fromEuler(tempQuat, 0.5 * Math.cos(angle), 0.5 * Math.sin(angle), 0.0);
            quat.invert(invQuat, tempQuat);
            quat.multiply(curViewDir, tempQuat, quat.multiply(curViewDir, curViewDir, invQuat));
            viewDir = vec3.fromValues(curViewDir[0], curViewDir[1], curViewDir[2]);
            break;

            // Pitch down
            case 'ArrowDown':
            quat.fromEuler(tempQuat, -0.5 * Math.cos(angle), -0.5 * Math.sin(angle), 0.0);
            quat.invert(invQuat, tempQuat);
            quat.multiply(curViewDir, tempQuat, quat.multiply(curViewDir, curViewDir, invQuat));
            viewDir = vec3.fromValues(curViewDir[0], curViewDir[1], curViewDir[2]);
            break;

            default:
            return; // Quit when this doesn't handle the key event.
        }

        // Cancel the default action to avoid it being handled twice
        event.preventDefault();
    }, true);
}


//----------------------------------------------------------------------------------
/**
 * Startup function called from html code to start program.
 */
function startup() {
    canvas = document.getElementById("myGLCanvas");
    gl = createGLContext(canvas);
    setupShaders("shader-phong-phong-vs","shader-phong-phong-fs");
    setupBuffers();
    gl.clearColor(1.0, 1.0, 1.0, 1.0);
    gl.enable(gl.DEPTH_TEST);
    //setupTextures();
    tick();
}

//----------------------------------------------------------------------------------
/**
 * Tick called for every animation frame.
 */
function tick() {
    requestAnimFrame(tick);
    draw();
}
