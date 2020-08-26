#version 440  
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D ourTexture;

//!#include "Common.glsl"

void main()
{
    FragColor = texture(ourTexture, TexCoord).rgba;
	
	FragColor.r = sin(Time);
}