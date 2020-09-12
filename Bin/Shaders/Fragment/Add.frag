#version 440  
out vec4 FragColor;

in vec2 TexCoord;

layout(binding=0) uniform sampler2D ourTexture;
layout(binding=1) uniform sampler2D ourTexture2;

//!#include "Common.glsl"

void main()
{
	FragColor = texture(ourTexture, TexCoord);  
    FragColor += texture(ourTexture2, TexCoord);  
}