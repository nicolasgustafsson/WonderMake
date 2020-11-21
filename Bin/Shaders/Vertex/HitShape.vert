#version 440
layout (location = 0) in vec2 aPos;
layout (location = 1) in float aUV;
layout (location = 2) in float aProgress;


out float uv;
out float progress;
out vec2 coord;

//!#include "Common.glsl"

void main() 
{
  vec3 transformedPosition = ViewProjectionMatrix * vec3(aPos.x, aPos.y, 1.0);
  
  gl_Position.xy = transformedPosition.xy;

  gl_Position.z = 0.0;
  gl_Position.w = 1.0;
  
  uv = aUV;
  progress = aProgress;
  
  coord = transformedPosition.xy;
};