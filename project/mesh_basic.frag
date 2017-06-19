#version 330
in float passLightValue;
uniform float alpha;
out vec4 outFragmentColor;

void main()
{
  outFragmentColor = vec4(vec3(1.0, 1.0, 1.0) * passLightValue, alpha);
}
