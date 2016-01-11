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
    //gl_FragColor = vColor;

    /*vec3 normal = normalize(vNormal);
    vec3 lightDir = normalize(lightPos - vPosition);

    float lambertian = max(dot(lightDir, normal), 0.0);
    float specular = 0.0;

    if (lambertian > 0.0)
    {
        vec3 reflectDir = reflect(-lightDir, normal);
        vec3 viewDir = normalize(-vPosition);

        float specAngle = max(dot(reflectDir, viewDir), 0.0);
        specular = pow(specAngle, 4.0);

        //specular *= lambertian;
    }*/

    //gl_FragColor = vec4(lambertian * baseColor + specular * specColor, 1.0);

    float distance = length(lightPos - vPosition);
    vec3 lightVector = normalize(lightPos - vPosition);
    float diffuse = max(dot(vNormal, lightVector), 0.1);
    diffuse = diffuse * (1.0 / (0.0000025 * distance * distance));
    //diffuse = max(diffuse, 0.3);
    //diffuse = min(diffuse, 0.7);
    gl_FragColor = vec4(baseColor * diffuse, 1.0);
}
