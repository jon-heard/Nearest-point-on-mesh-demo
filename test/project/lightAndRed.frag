#version 330
in float passLightValue;
out vec4 outFragmentColor;

void main()
{
  outFragmentColor = vec4(passLightValue, 0.0, 0.0, 1.0);
}
