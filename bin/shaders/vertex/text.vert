#version 440
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 vertexScale;
out vec2 TexCoord;

//!#include "common.glsl"

void main() 
{

  vec3 transformedPosition = vec3(aPos.x, aPos.y, 1.0) * ViewProjectionMatrix;
  gl_Position = vec4(transformedPosition.xyz, 1.0);
  //gl_Position = vec4(CorrectSubpixelPositioning(transformedPosition.xy), 1.0, 1.0);
  
  TexCoord = aTexCoord;
};