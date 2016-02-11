#ifdef GL_ES
  precision mediump float;
#endif

//varying vec4 vColor;
varying vec3 vLightNorm;
varying vec3 vPos;

const vec3 baseColor = vec3(0.2, 0.2, 0.8);
const vec3 lightPos = vec3(100.0, 100.0, 200.0);
const vec3 norm = vec3(0.0, 0.0, 1.0);

void main()
{
    vec3 lightVector = normalize(lightPos - vPos);
    //float diffuse = max(dot(vLightNorm, lightVector), 0.1);
    float diffuse = max(dot(norm, lightVector), 0.1);
    gl_FragColor = vec4(baseColor * diffuse, 1.0);
}

