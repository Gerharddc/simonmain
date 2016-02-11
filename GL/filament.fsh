#ifdef GL_ES
  precision mediump float;
#endif

varying vec4 vColor;

void main()
{
    gl_FragColor = vColor;
}

// TODO: add depth for cylinder
