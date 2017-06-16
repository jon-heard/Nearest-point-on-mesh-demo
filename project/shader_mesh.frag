#version 330
in float passLightValue;
uniform float transparency;
out vec4 finalColor;

void main()
{
  finalColor = vec4(vec3(1, 1, 1) * passLightValue, transparency);
}
