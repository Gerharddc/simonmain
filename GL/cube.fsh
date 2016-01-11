#ifdef GL_ES
  precision mediump float;
#endif

//varying vec4 vColor;

varying vec3 vPosition;
varying vec3 vNormal;
varying vec3 lightPos;

const vec3 baseColor = vec3(1.0, 0.0, 0.0);
const vec3 specColor = vec3(1.0, 1.0, 1.0);

void main()
{
    float distance = length(lightPos - vPosition);
    vec3 lightVector = normalize(lightPos - vPosition);
    float diffuse = max(dot(vNormal, lightVector), 0.1);
    //diffuse = diffuse * (1.0 / (1.0 + (0.00001 * distance * distance)));
    diffuse += 0.2;
    //diffuse = max(diffuse, 0.3);
    //diffuse = min(diffuse, 0.7);
    gl_FragColor = vec4(baseColor * diffuse, 1.0);

    //gl_FragColor = vColor;
}
