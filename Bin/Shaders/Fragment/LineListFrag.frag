#version 440
out vec4 FragColor;
  
in vec2 TexCoord;

uniform vec4 Color = vec4(0.0, 1.0, 1.0, 1.0);

//!#include "Common.glsl"

void main()
{
    FragColor = Color;
}