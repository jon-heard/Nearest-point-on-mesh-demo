#version 120

varying float passLightValue;

void main()
{
  gl_FragColor = vec4(0.0, 0.0, passLightValue, 1.0);
}
