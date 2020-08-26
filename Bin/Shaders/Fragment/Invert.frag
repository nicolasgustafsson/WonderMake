#version 440  
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D ourTexture;

//!#include "Common.glsl"

void main()
{
    FragColor = texture(ourTexture, TexCoord).rgba;
	
	FragColor.rgb = vec3(1.0, 1.0, 1.0) - FragColor.rgb;
}