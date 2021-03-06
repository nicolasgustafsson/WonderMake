#version 440
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec4 aColor;

out vec4 vertexColor;

//!#include "Common.glsl"

void main() 
{
  vec3 transformedPosition = vec3(aPos.x, aPos.y, 1.0) * ViewProjectionMatrix;
  
  gl_Position.xy = CorrectSubpixelPositioning(transformedPosition.xy);

  gl_Position.z = 0.0;
  gl_Position.w = 1.0;
  
  vertexColor = aColor;
};