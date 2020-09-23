#version 440  
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D ourTexture;

//!#include "Common.glsl"

void main()
{
	vec2 texCoordYFlipped = TexCoord;
	
	texCoordYFlipped.y = 1.0 - texCoordYFlipped.y;
    FragColor = texture(ourTexture, texCoordYFlipped).rgba;  
}