#version 120

varying float passLightValue;
varying vec2 passTextureCoordinates;

uniform float alpha;
uniform sampler2D textureSampler;

void main()
{
  vec4 baseColor = vec4(vec3(1, 1, 1) * passLightValue, 1.0);
  vec4 textureColor = texture2D(textureSampler, passTextureCoordinates);
  gl_FragColor = vec4(
      mix(baseColor.rgb, textureColor.rgb, textureColor.a), alpha + textureColor.a);
}
