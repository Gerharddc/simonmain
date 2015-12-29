#ifdef GL_ES
  precision mediump float;
#endif

varying vec4 vColor;
void main()
{
    float grad = gl_FragCoord.x / 480.0f;
    gl_FragColor = vColor;
    gl_FragColor = vec4(vColor.x, vColor.y * grad, vColor.z * grad, vColor.w);
}
