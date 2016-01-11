uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjMatrix;
uniform mat4 uNormMatrix;

attribute vec4 aPosition;
attribute vec4 aNormal;

//varying vec4 vColor;

varying vec3 vPosition;
varying vec3 vNormal;
varying vec3 lightPos;

const vec4 lightPosBase = vec4(100.0, 100.0, 100.0, 0.0);
//const vec3 baseColor = vec3(1.0, 0.5, 0.25);

void main()
{
    vPosition = vec3(uViewMatrix * uModelMatrix * aPosition);
    vNormal = vec3(uNormMatrix * aNormal);
    lightPos = vec3(1.0 * lightPosBase);

    gl_Position = uProjMatrix * uViewMatrix * uModelMatrix * aPosition;
}
