#version 120

varying float passLightValue;

void main()
{
  gl_FragColor = vec4(passLightValue, 0.0, 0.0, 1.0);
}
