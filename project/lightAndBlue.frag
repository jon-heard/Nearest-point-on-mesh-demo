#version 120

// Fragment - Blue, adjusted by light value.

varying float passLightValue;

void main()
{
  gl_FragColor = vec4(0.0, 0.0, passLightValue, 1.0);
}
