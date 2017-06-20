#version 120

// Fragment - Red, adjusted by light value.

varying float passLightValue;

void main()
{
  gl_FragColor = vec4(passLightValue, 0.0, 0.0, 1.0);
}
