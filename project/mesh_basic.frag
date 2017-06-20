#version 120

// Fragment - White, adjusted by light and alpha values.

varying float passLightValue;
uniform float alpha;

void main()
{
  gl_FragColor = vec4(vec3(1.0, 1.0, 1.0) * passLightValue, alpha);
}
