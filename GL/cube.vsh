uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjMatrix;
attribute vec4 aPosition;
varying vec4 vColor;

const vec3 lightPos = vec3(1.0, 1.0, 1.0);
const vec3 diffuseColor = vec3(0.5, 0.0, 0.0);
const vec3 specColor = vec3(1.0, 1.0, 1.0);

void main()
{
    gl_Position = uProjMatrix * uViewMatrix * uModelMatrix * aPosition;
    vColor = vec4(1.0, 0.5, 0.1, 0.5);
}
