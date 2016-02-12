#ifdef GL_ES
  precision mediump float;
#else
    #define USE_Z
#endif

#extension EXT_frag_depth: enable
#ifdef GL_EXT_frag_depth
    #define USE_Z
#endif

varying vec4 vColor;

#ifdef USE_Z
varying float vZOff;
#endif

void main()
{
    gl_FragColor = vColor;

    // TODO: these distances might need scaling
#ifndef GL_ES
    gl_FragDepth = gl_FragCoord.z + abs(vZOff);
#endif
#ifdef GL_EXT_frag_depth
    gl_FragDepthEXT = gl_FragCoord.z + abs(vZOff);
#endif
}

// TODO: the depths to add might not have been calculated correctly for intersecting points
