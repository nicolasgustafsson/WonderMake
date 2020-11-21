#version 440
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aOrigin;
layout (location = 2) in vec2 aScale;
layout (location = 3) in vec4 aColor;
layout (location = 4) in float aRotation;

out vec2 vertexScale;
out vec2 vertexOrigin;
out vec4 vertexColor;
out float vertexRotation;

//!#include "Common.glsl"

void main() 
{
  gl_Position = vec4(aPos.x, aPos.y, 1.0, 1.0);
  
  vertexScale = aScale;
  vertexColor = aColor;
  vertexRotation = aRotation;
  vertexOrigin = aOrigin;
};