#version 120

attribute vec3 position;
attribute vec3 normal;

uniform mat4 transform_projection_camera_model;
uniform mat4 transform_camera_model;

uniform mat4 adjust_projection_focusCam_model_transform;

varying float passLightValue;
varying vec4 passTextureCoordinates;

void main()
{
  // Run normal transformation and lighting
  gl_Position = transform_projection_camera_model * vec4(position, 1.0);
  passLightValue = dot(
      normalize(transform_camera_model * vec4(normal, 0.0)), vec4(0.0, 0.0, 1.0, 0.0));

  passTextureCoordinates = vec4(
      adjust_projection_focusCam_model_transform * vec4(position, 1.0));
}
