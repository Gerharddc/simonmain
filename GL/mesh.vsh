uniform mat4 uModelMatrix;
uniform mat4 uProjMatrix;
uniform mat4 uNormMatrix;

attribute vec4 aPosition;
attribute vec3 aNormal;

varying vec4 vColor;

const vec3 lightPos = vec3(50.0, 50.0, 150.0);
const vec3 baseColor = vec3(1.0, 0.0, 0.0);

void main()
{
    vec3 vPosition = vec3(uModelMatrix * aPosition);
    vec3 vNormal = vec3(uNormMatrix * vec4(aNormal, 0.0));
    vec3 lightVector = normalize(lightPos - vPosition);
    float diffuse = max(dot(vNormal, lightVector), 0.1);
    vColor = vec4(baseColor * diffuse, 1.0);

    gl_Position = uProjMatrix * uModelMatrix * aPosition;
}
