#version 330
in float passLightValue;
in vec2 texCoords;

uniform sampler2D decal;

out vec4 finalColor;

void main()
{
  vec4 baseColor = vec4(vec3(.5, .5, .5) * passLightValue, 0.6);
  vec4 textureColor = texture(decal, texCoords);
  finalColor = vec4(mix(baseColor.rgb, textureColor.rgb, textureColor.a), 0.6);
}
