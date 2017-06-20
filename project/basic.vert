#version 120

// Vertex - Simply calculates geometry and normals

attribute vec3 position;
attribute vec3 normal;

uniform mat4 transform_projection_camera_model;
uniform mat4 transform_camera_model;

varying float passLightValue;

void main()
{
  gl_Position = transform_projection_camera_model * vec4(position, 1.0);
  passLightValue = dot(
      normalize(transform_camera_model * vec4(normal, 0.0)), vec4(0.0, 0.0, 1.0, 0.0));
}
