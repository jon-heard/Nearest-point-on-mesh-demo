#version 330
in float passLightValue;

out vec4 finalColor;

void main()
{
  finalColor = vec4(vec3(1, 1, 1) * passLightValue, 0.6);
}
