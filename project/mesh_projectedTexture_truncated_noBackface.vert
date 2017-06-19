#version 330
layout(location=1) in vec3 position;
layout(location=2) in vec3 normal;

uniform mat4 projectionCameraTransform;
uniform mat4 cameraTransform;
uniform mat4 modelTransform;

uniform mat4 decalAdjustAndProjectionTransform;
uniform mat4 decalCameraTransform;
uniform vec3 decalNormal;

uniform vec3 nearestPoint;
uniform float scale;

out float passLightValue;
out vec2 passTextureCoordinates;

void main()
{
  // Run normal transformation and lighting
  gl_Position = projectionCameraTransform * modelTransform * vec4(position, 1.0);
  passLightValue = dot(
      normalize(cameraTransform * modelTransform * vec4(normal, 0.0)), vec4(0.0, 0.0, 1.0, 0.0));

  // Run projected texture mapping
  vec4 projectedTextureCoordinates = vec4(
      decalAdjustAndProjectionTransform * decalCameraTransform *
      modelTransform * vec4(position, 1.0));

  // Scale texture coordintaes
  passTextureCoordinates = projectedTextureCoordinates.st / projectedTextureCoordinates.q;
  float distanceFromCenter =
      length(modelTransform * vec4(position, 1.0) - vec4(nearestPoint, 1.0)) / scale;
  passTextureCoordinates =
      normalize(passTextureCoordinates - vec2(.5, .5)) * distanceFromCenter + vec2(.5, .5);

  // If projecting onto a backface, prevent projecting
  if (dot(normalize(modelTransform * vec4(normal, 0.0)), vec4(decalNormal, 0.0)) > 0)
  {
    passTextureCoordinates = vec2(0,0);
  }
}
