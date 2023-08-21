#version 440
out vec4 FragColor;
  
in vec2 TexCoord;
in vec4 Color;

uniform sampler2D ourTexture;

//!#include "common.glsl"

void main()
{
	vec4 texColor = texture(ourTexture, TexCoord);
    FragColor = texColor * Color;
} 