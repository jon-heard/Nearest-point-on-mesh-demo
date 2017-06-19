#version 330
layout(location=1) in vec3 position;
layout(location=2) in vec3 normal;

uniform mat4 projectionCameraTransform;
uniform mat4 cameraTransform;
uniform mat4 modelTransform;

uniform vec3 nearestPoint;
uniform float scale;
uniform mat4 lookatTransform;
uniform vec3 decalNormal;

out float passLightValue;
out vec2 passTextureCoordinates;

void main()
{
  gl_Position = projectionCameraTransform * modelTransform * vec4(position, 1.0);
  passLightValue = dot(
      normalize(cameraTransform * modelTransform * vec4(normal, 0.0)),
      vec4(0.0, 0.0, 1.0, 0.0));

  vec4 vectorFromCenter = vec4(nearestPoint, 0.0) - modelTransform * vec4(position, 1.0);
  vectorFromCenter = lookatTransform * vectorFromCenter;
  passTextureCoordinates = vectorFromCenter.xy;
  passTextureCoordinates = passTextureCoordinates / scale + vec2(.5,.5);
  // If projecting onto a backface, don't project
//  if (dot(normalize(modelTransform * vec4(normal, 0.0)), vec4(decalNormal, 0.0)) > 0)
//  {
//    passTextureCoordinates = vec2(0,0);
//  }
}
