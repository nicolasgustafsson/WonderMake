#version 440

out vec4 FragColor;
  
in vec2 TexCoord;
in vec4 Color;

uniform sampler2D ourTexture;

//!#include "SimplexNoise.glsl"
//!#include "Common.glsl"

void main()
{
	float dist = ((distance(TexCoord, vec2(0.5, 0.5)) * 2.0) + 0.5f) / 1.5f;
	float noise = simplex3d(vec3(TexCoord * 15.f, Time));
	dist += noise * 0.1;
    float withinCircle = 1.0 - smoothstep(dist + 0.01f, dist, 1.0);
     
    float renderAlpha = min(withinCircle, dist + simplex3d(vec3(TexCoord * 45.f, Time * 0.1)) * 0.1);

	vec4 color = Color;
	
    // Output to screen
    FragColor = color * renderAlpha;
}