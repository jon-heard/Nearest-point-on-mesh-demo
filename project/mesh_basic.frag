#version 330
in float passLightValue;
uniform float alpha;
out vec4 outFragmentClor;

void main()
{
  outFragmentClor = vec4(vec3(1, 1, 1) * passLightValue, alpha);
}
