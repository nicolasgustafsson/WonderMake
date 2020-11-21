#version 440  
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D ourTexture;

//!#include "Common.glsl"

vec2 texelSize = vec2(1.0 / (Resolution.x), 1.0 / (Resolution.y));
float weights[7] = { 0.129001, 0.142521, 0.151303, 0.15435, 0.151303, 0.142521, 0.129001 };
float texelOffset[7] = { -3.0, -2.0, -1.0, 0.0, 1.0, 2.0, 3.0 }; 
float texelOffsetMultiplier = 1.0;

void main()
{
	vec4 blurColour = vec4(0, 0, 0, 0);

	for (uint i = 0; i < 7; ++i)
	{
		vec2 textureCoords = TexCoord;
		textureCoords.y += texelSize.y * texelOffset[i] * texelOffsetMultiplier;
		vec4 textureSample = texture(ourTexture, textureCoords).rgba;
		blurColour += textureSample * weights[i];
	}

	FragColor = blurColour;
}