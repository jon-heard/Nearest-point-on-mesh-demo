#version 330
in float passLightValue;
in vec4 texCoords;

uniform float transparency;
uniform sampler2D decal;

out vec4 finalColor;

void main()
{
  vec4 baseColor = vec4(vec3(1, 1, 1) * passLightValue, 1.0);
  vec4 textureColor = texture2DProj(decal, texCoords);
  finalColor = vec4(mix(baseColor.rgb, textureColor.rgb, textureColor.a), transparency + textureColor.a);
}
