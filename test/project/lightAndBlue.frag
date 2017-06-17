#version 330
in float passLightValue;
out vec4 outFragmentColor;

void main()
{
  outFragmentColor = vec4(0.0, 0.0, passLightValue, 1.0);
}
