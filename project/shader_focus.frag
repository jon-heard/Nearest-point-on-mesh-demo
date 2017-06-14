#version 330
in float passLightValue;

out vec4 finalColor;

void main()
{
  finalColor = vec4(passLightValue, 0.0, 0.0, 1.0);
}
