#version 440  
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D ourTexture;
uniform sampler2D alphaMask;

//!#include "Common.glsl"

void main()
{
    FragColor.rgb = texture(ourTexture, TexCoord).rgb;  
	FragColor.a = texture(alphaMask, TexCoord).r;  
}