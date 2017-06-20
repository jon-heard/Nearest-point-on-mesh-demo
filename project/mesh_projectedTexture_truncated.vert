#version 120

// Vertex - Projected texture mapping with texcoord distance scaled to distance from decal center.

attribute vec3 position;
attribute vec3 normal;

uniform mat4 transform_projection_camera_model;
uniform mat4 transform_camera_model;

uniform mat4 adjust_projection_focusCam_model_transform;
uniform mat4 transform_model;
uniform vec3 nearestPoint;
uniform float scale;

varying float passLightValue;
varying vec2 passTextureCoordinates;

void main()
{
  // Run normal transformation and lighting
  gl_Position = transform_projection_camera_model * vec4(position, 1.0);
  passLightValue = dot(
      normalize(transform_camera_model * vec4(normal, 0.0)), vec4(0.0, 0.0, 1.0, 0.0));

  // Run projected texture mapping
  vec4 projectedTextureCoordinates = vec4(
      adjust_projection_focusCam_model_transform * vec4(position, 1.0));
  passTextureCoordinates = projectedTextureCoordinates.st / projectedTextureCoordinates.q;

  // Scale texture coordintaes
  float distanceFromCenter =
      length(transform_model * vec4(position, 1.0) - vec4(nearestPoint, 1.0)) / scale;
  passTextureCoordinates =
      normalize(passTextureCoordinates - vec2(.5, .5)) * distanceFromCenter + vec2(.5, .5);
}
