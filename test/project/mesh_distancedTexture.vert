#version 330
layout(location=1) in vec3 position;
layout(location=2) in vec3 normal;

uniform mat4 projectionCameraTransform;
uniform mat4 cameraTransform;
uniform mat4 modelTransform;

uniform vec3 focus;
uniform vec3 nearestPoint;

out float passLightValue;
out vec2 passTextureCoordinates;

void main()
{
  gl_Position = projectionCameraTransform * modelTransform * vec4(position, 1.0);
  passLightValue = dot(
      normalize(cameraTransform * modelTransform * vec4(normal, 0.0)),
      vec4(0.0, 0.0, 1.0, 0.0));
  vec4 a = modelTransform * vec4(position, 1.0) - vec4(nearestPoint, 0.0);
  float b = length(a) / length(focus - nearestPoint);
  a = normalize(a);
  passTextureCoordinates = vec2(b * a.x + .5, b * a.y + .5);
}
