
var gl;
var canvas;
var shaderProgram;
var vertexPositionBuffer;

var days=0;

// Create a place to store sphere geometry
var sphereVertexPositionBuffer;

//Create a place to store normals for shading
var sphereVertexNormalBuffer;

// View parameters
var eyePt = vec3.fromValues(0.0,0.0,10.0);
var viewDir = vec3.fromValues(0.0,0.0,-100.0);
var up = vec3.fromValues(0.0,1.0,0.0);
var viewPt = vec3.fromValues(0.0,0.0,0.0);

// Create the normal
var nMatrix = mat3.create();

// Create ModelView matrix
var mvMatrix = mat4.create();

//Create Projection matrix
var pMatrix = mat4.create();

var mvMatrixStack = [];

// store the positions of spheres.
var positionArray = [];
// store the velocities of spheres.
var velocityArray = [];
// store the radius of spheres.
var radiusArray = [];
// store the colors of spheres.
var colorArray = [];

// gravitational acceleration.
const G = 0.01;
// frictional coefficient.
const F = 0.0001;
// timer.
var timer = 0.0;

//-------------------------------------------------------------------------
function setupSphereBuffers() {
    var sphereSoup=[];
    var sphereNormals=[];
    var numT = sphereFromSubdivision(6, sphereSoup, sphereNormals);
    console.log("Generated ", numT, " triangles");
    sphereVertexPositionBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, sphereVertexPositionBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(sphereSoup), gl.STATIC_DRAW);
    sphereVertexPositionBuffer.itemSize = 3;
    sphereVertexPositionBuffer.numItems = numT*3;
    console.log(sphereSoup.length/9);

    // Specify normals to be able to do lighting calculations
    sphereVertexNormalBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, sphereVertexNormalBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(sphereNormals),
                  gl.STATIC_DRAW);
    sphereVertexNormalBuffer.itemSize = 3;
    sphereVertexNormalBuffer.numItems = numT*3;

    console.log("Normals ", sphereNormals.length/3);
}

//-------------------------------------------------------------------------
function drawSphere() {
    gl.bindBuffer(gl.ARRAY_BUFFER, sphereVertexPositionBuffer);
    gl.vertexAttribPointer(shaderProgram.vertexPositionAttribute, sphereVertexPositionBuffer.itemSize,
                         gl.FLOAT, false, 0, 0);

    // Bind normal buffer
    gl.bindBuffer(gl.ARRAY_BUFFER, sphereVertexNormalBuffer);
    gl.vertexAttribPointer(shaderProgram.vertexNormalAttribute,
                           sphereVertexNormalBuffer.itemSize,
                           gl.FLOAT, false, 0, 0);
    gl.drawArrays(gl.TRIANGLES, 0, sphereVertexPositionBuffer.numItems);
}

//-------------------------------------------------------------------------
function uploadModelViewMatrixToShader() {
    gl.uniformMatrix4fv(shaderProgram.mvMatrixUniform, false, mvMatrix);
}

//-------------------------------------------------------------------------
function uploadProjectionMatrixToShader() {
    gl.uniformMatrix4fv(shaderProgram.pMatrixUniform, false, pMatrix);
}

//-------------------------------------------------------------------------
function uploadNormalMatrixToShader() {
    mat3.fromMat4(nMatrix,mvMatrix);
    mat3.transpose(nMatrix,nMatrix);
    mat3.invert(nMatrix,nMatrix);
    gl.uniformMatrix3fv(shaderProgram.nMatrixUniform, false, nMatrix);
}

//----------------------------------------------------------------------------------
function mvPushMatrix() {
    var copy = mat4.clone(mvMatrix);
    mvMatrixStack.push(copy);
}


//----------------------------------------------------------------------------------
function mvPopMatrix() {
    if (mvMatrixStack.length == 0) {
        throw "Invalid popMatrix!";
    }
    mvMatrix = mvMatrixStack.pop();
}

//----------------------------------------------------------------------------------
function setMatrixUniforms() {
    uploadModelViewMatrixToShader();
    uploadNormalMatrixToShader();
    uploadProjectionMatrixToShader();
}

//----------------------------------------------------------------------------------
function degToRad(degrees) {
    return degrees * Math.PI / 180;
}

//----------------------------------------------------------------------------------
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

    shaderProgram.vertexNormalAttribute = gl.getAttribLocation(shaderProgram, "aVertexNormal");
    gl.enableVertexAttribArray(shaderProgram.vertexNormalAttribute);

    shaderProgram.mvMatrixUniform = gl.getUniformLocation(shaderProgram, "uMVMatrix");
    shaderProgram.pMatrixUniform = gl.getUniformLocation(shaderProgram, "uPMatrix");
    shaderProgram.nMatrixUniform = gl.getUniformLocation(shaderProgram, "uNMatrix");

    shaderProgram.uniformLightPositionLoc = gl.getUniformLocation(shaderProgram, "uLightPosition");
    shaderProgram.uniformAmbientLightColorLoc = gl.getUniformLocation(shaderProgram, "uAmbientLightColor");
    shaderProgram.uniformDiffuseLightColorLoc = gl.getUniformLocation(shaderProgram, "uDiffuseLightColor");
    shaderProgram.uniformSpecularLightColorLoc = gl.getUniformLocation(shaderProgram, "uSpecularLightColor");

    shaderProgram.uniformAmbientMatColorLoc = gl.getUniformLocation(shaderProgram, "uAmbientMatColor");
    shaderProgram.uniformDiffuseMatColorLoc = gl.getUniformLocation(shaderProgram, "uDiffuseMatColor");
    shaderProgram.uniformSpecularMatColorLoc = gl.getUniformLocation(shaderProgram, "uSpecularMatColor");
}


//-------------------------------------------------------------------------
function uploadLightsToShader(loc,a,d,s) {
    gl.uniform3fv(shaderProgram.uniformLightPositionLoc, loc);
    gl.uniform3fv(shaderProgram.uniformAmbientLightColorLoc, a);
    gl.uniform3fv(shaderProgram.uniformDiffuseLightColorLoc, d);
    gl.uniform3fv(shaderProgram.uniformSpecularLightColorLoc, s);
}

//-------------------------------------------------------------------------
function uploadMaterialToShader(a,d,s) {
    gl.uniform3fv(shaderProgram.uniformAmbientMatColorLoc, a);
    gl.uniform3fv(shaderProgram.uniformDiffuseMatColorLoc, d);
    gl.uniform3fv(shaderProgram.uniformSpecularMatColorLoc, s);
}


//----------------------------------------------------------------------------------
function setupBuffers() {
    setupSphereBuffers();
}

//----------------------------------------------------------------------------------
function draw() {
    timer += 0.001;
    keyListener();
    var transformVec = vec3.create();

    gl.viewport(0, 0, gl.viewportWidth, gl.viewportHeight);
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    // We'll use perspective
    mat4.perspective(pMatrix,degToRad(45), gl.viewportWidth / gl.viewportHeight, 0.1, 200.0);

    // We want to look down -z, so create a lookat point in that direction
    vec3.add(viewPt, eyePt, viewDir);
    // Then generate the lookat matrix and initialize the MV matrix to that view
    mat4.lookAt(mvMatrix,eyePt,viewPt,up);

    var lightPosEye4 = vec4.fromValues(0.0,50.0,50.0,1.0);
    lightPosEye4 = vec4.transformMat4(lightPosEye4,lightPosEye4,mvMatrix);
    //console.log(vec4.str(lightPosEye4))
    var lightPosEye = vec3.fromValues(lightPosEye4[0], lightPosEye4[1], lightPosEye4[2]);

    // Set up light parameters
    var Ia = vec3.fromValues(1.0,1.0,1.0);
    var Id = vec3.fromValues(1.0,1.0,1.0);
    var Is = vec3.fromValues(1.0,1.0,1.0);

    for (var i = 0; i < positionArray.length; i+=3) {
        // Set up material parameters
        var ka = vec3.fromValues(0.0,0.0,0.0);
        var kd = vec3.fromValues(colorArray[i],colorArray[i+1],colorArray[i+2]);
        var ks = vec3.fromValues(colorArray[i],colorArray[i+1],colorArray[i+2]);

        mvPushMatrix();
        var transformVec, scaleVec;
        vec3.set(transformVec, positionArray[i], positionArray[i+1], positionArray[i+2]);
        mat4.translate(mvMatrix, mvMatrix, transformVec);

        scaleVec = vec3.fromValues(radiusArray[i], radiusArray[i+1], radiusArray[i+2]);
        mat4.scale(mvMatrix, mvMatrix, scaleVec);

        uploadLightsToShader(lightPosEye, Ia, Id, Is);
        uploadMaterialToShader(ka, kd, ks);
        setMatrixUniforms();
        drawSphere();

        positionArray[i+0] += velocityArray[i+0];
        positionArray[i+1] += velocityArray[i+1];
        positionArray[i+2] += velocityArray[i+2];

        if (positionArray[i+1] - radiusArray[i+1] <= -2.0) {
            positionArray[i+1] = radiusArray[i+1]-2.0;
            velocityArray[i+1] = -velocityArray[i+1];
        } else if (positionArray[i+1] + radiusArray[i+1] >= 2.0) {
            positionArray[i+1] = 2.0 - radiusArray[i+1];
            velocityArray[i+1] = -velocityArray[i+1];
        }

        if (positionArray[i] - radiusArray[i] <= -2.0) {
            positionArray[i] = radiusArray[i]-2.0;
            velocityArray[i] = -velocityArray[i];
        } else if (positionArray[i] + radiusArray[i] >= 2.0) {
            positionArray[i] = 2.0 - radiusArray[i];
            velocityArray[i] = -velocityArray[i];
        }

        if (positionArray[i+2] - radiusArray[i+2] <= -2.0) {
            positionArray[i+2] = radiusArray[i+2]-2.0;
            velocityArray[i+2] = -velocityArray[i+2];
        } else if (positionArray[i+2] + radiusArray[i+2] >= 2.0) {
            positionArray[i+2] = 2.0 - radiusArray[i+2];
            velocityArray[i+2] = -velocityArray[i+2];
        }

        if (velocityArray[i+1] > 0) {
            velocityArray[i+1] -= G;
            velocityArray[i+1] -= F;
        } else if (velocityArray[i+1] < 0) {
            velocityArray[i+1] -= G;
            velocityArray[i+1] += F;
        }

        if (velocityArray[i] > 0) {
            velocityArray[i] -= F;
        } else if (velocityArray[i] < 0) {
            velocityArray[i] += F;
        }
        if (velocityArray[i+2] > 0) {
            velocityArray[i+2] -= F;
        } else if (velocityArray[i+2] < 0) {
            velocityArray[i+2] += F;
        }

        mvPopMatrix();
    }
}

/**
 * Listen the key press to do the rotation.
 */
function keyListener() {
    var velocityRange = 0.1;
    window.addEventListener("keydown", function (event) {
        if (event.defaultPrevented) {
            return; // Do nothing if the event was already processed
        }

        switch (event.key) {
            // add a ball.
            case "a":
            // init position array.
            positionArray.push(Math.random()*2.0-1.0);
            positionArray.push(Math.random()*0.5+0.5);
            positionArray.push(Math.random()*2.0-1.0);

            //init velocity array.
            var initSpeedX = (Math.random()-0.5)*velocityRange;
            var initSpeedY = (Math.random()-0.5)*velocityRange;
            var initSpeedZ = (Math.random()-0.5)*velocityRange;
            velocityArray.push(initSpeedX);
            velocityArray.push(initSpeedY);
            velocityArray.push(initSpeedZ);

            // init radius array.
            var randomRadius = Math.random()/3.0+0.3;
            radiusArray.push(randomRadius);
            radiusArray.push(randomRadius);
            radiusArray.push(randomRadius);

            // init color array.
            colorArray.push(Math.random());
            colorArray.push(Math.random());
            colorArray.push(Math.random());
            break;

            // press "d" to remove all the spheres.
            case "d":
            positionArray = [];
            velocityArray = [];
            radiusArray = [];
            colorArray = [];
            break;


            default:
            return; // Quit when this doesn't handle the key event.
        }

        // Cancel the default action to avoid it being handled twice
        event.preventDefault();
    }, true);
}


//----------------------------------------------------------------------------------
function startup() {
    canvas = document.getElementById("myGLCanvas");
    gl = createGLContext(canvas);
    setupShaders();
    setupBuffers();
    gl.clearColor(0.0, 1.0, 1.0, 1.0);
    gl.enable(gl.DEPTH_TEST);
    tick();
}

//----------------------------------------------------------------------------------
function tick() {
    requestAnimFrame(tick);
    draw();
}
