#version 330
in float passLightValue;
out vec4 finalColor;

void main()
{
  finalColor = vec4(0.0, 0.0, passLightValue, 1.0);
}
