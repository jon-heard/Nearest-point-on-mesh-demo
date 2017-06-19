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

  vec4 vectorFromCenter = modelTransform * vec4(position, 1.0) - vec4(nearestPoint, 0.0);
  vec3 lookAtVector = nearestPoint - focus;
  float scaledDistanceFromCenter = length(vectorFromCenter) / length(lookAtVector);
  vec4 normalizedVectorFromCenter = normalize(vectorFromCenter);
  vec3 normalizedLookAt = normalize(lookAtVector);
  passTextureCoordinates = vec2(
      scaledDistanceFromCenter * normalizedVectorFromCenter.x + .5,
      scaledDistanceFromCenter * normalizedVectorFromCenter.y + .5);
}
