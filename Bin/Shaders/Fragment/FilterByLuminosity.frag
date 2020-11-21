#version 440  
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D ourTexture;

//!#include "Common.glsl"

void main()
{
    FragColor = texture(ourTexture, TexCoord).rgba;  
	
	float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 0.8)
        FragColor = vec4(FragColor.rgb, 1.0);
    else
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
		
}