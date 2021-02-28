#version 440  
out vec4 FragColor;

in vec2 TexCoord;

layout(binding=0) uniform sampler2D ourTexture;
layout(binding=1) uniform sampler2D alphaMask;

//!#include "Common.glsl"

void main()
{
    FragColor.rgb = texture(ourTexture, TexCoord).rgb;  
	FragColor.a = texture(alphaMask, TexCoord).r;  
}