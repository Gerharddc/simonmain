uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjMatrix;

attribute vec4 aPosition;

varying vec4 vColor;

const vec3 color = vec3(0.0, 0.0, 1.0);

void main(void)
{
    vColor = vec4(color, 1.0);
    gl_Position = uProjMatrix * uViewMatrix * uModelMatrix * aPosition;
}
