#version 440
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

//!#include "utility/engine_uniform_buffer.glsl"

out vec2 TexCoord;

void main() 
{
  gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
  TexCoord = aTexCoord;
};