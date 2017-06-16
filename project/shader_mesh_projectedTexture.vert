#version 330

layout(location=1) in vec3 position;
layout(location=2) in vec3 normal;

uniform mat4 projectionTransform;
uniform mat4 cameraTransform;
uniform mat4 modelTransform;

uniform mat4 decalProjection;
uniform mat4 decalCamera;
uniform mat4 decalAdjust;
uniform vec3 decalNormal;

out float passLightValue;
out vec4 texCoords;

void main()
{
  gl_Position = projectionTransform * cameraTransform * modelTransform * vec4(position, 1.0);
  passLightValue = dot(normalize(cameraTransform * modelTransform * vec4(normal, 0.0)), vec4(0.0, 0.0, 1.0, 0.0));
  texCoords = vec4(decalAdjust * decalProjection * decalCamera * modelTransform * vec4(position, 1.0)) * .01;
  if (dot(normalize(modelTransform * vec4(normal, 0.0)), vec4(decalNormal, 0.0)) > 0)
  {
    texCoords = vec4(0,0,0,0);
  }
}
