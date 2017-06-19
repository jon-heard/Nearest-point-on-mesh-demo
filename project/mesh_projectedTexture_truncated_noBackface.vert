#version 330
layout(location=1) in vec3 position;
layout(location=2) in vec3 normal;

uniform mat4 transform_projection_camera_model;
uniform mat4 transform_camera_model;

uniform mat4 adjust_projection_focusCam_model_transform;
uniform mat4 transform_model;
uniform vec3 nearestPoint;
uniform float scale;
uniform vec3 decalNormal;

out float passLightValue;
out vec2 passTextureCoordinates;

void main()
{
  // Run normal transformation and lighting
  gl_Position = transform_projection_camera_model * vec4(position, 1.0);
  passLightValue = dot(
      normalize(transform_camera_model * vec4(normal, 0.0)), vec4(0.0, 0.0, 1.0, 0.0));

  // Run projected texture mapping
  vec4 projectedTextureCoordinates = vec4(
      adjust_projection_focusCam_model_transform * vec4(position, 1.0));

  // Scale texture coordintaes
  passTextureCoordinates = projectedTextureCoordinates.st / projectedTextureCoordinates.q;
  float distanceFromCenter =
      length(transform_model * vec4(position, 1.0) - vec4(nearestPoint, 1.0)) / scale;
  passTextureCoordinates =
      normalize(passTextureCoordinates - vec2(.5, .5)) * distanceFromCenter + vec2(.5, .5);

  // If projecting onto a backface, prevent projecting
  if (dot(normalize(transform_model * vec4(normal, 0.0)), vec4(decalNormal, 0.0)) > 0)
  {
    passTextureCoordinates = vec2(0,0);
  }
}
