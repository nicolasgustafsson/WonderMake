#version 440

out vec4 FragColor;
  
in vec2 TexCoord;   
in vec4 Color;   
uniform sampler2D ourTexture;
 
//!#include "Common.glsl"  
  
void main() 
{ 
	float noise = simplex3d(vec3(TexCoord * 15.f, Time)); 
	float dist = ((distance(TexCoord, vec2(0.5, 0.5)) * 2.0) + 0.5f) / 1.42f;
	float labyrinth = labyrinthMoving(TexCoord * 30.f, Time) * 0.8 * step(-0.3, noise);
	//dist += labyrinth * 0.1;  
    float withinCircle = 1.0 - smoothstep(dist + 0.01f, dist, 1.0);
     
    float renderAlpha = labyrinth * withinCircle;//min(withinCircle, dist + simplex3d(vec3(TexCoord * 45.f, Time * 0.1)) * 0.1);

	vec4 color = Color;
	
    // Output to screen
    FragColor = color;
	FragColor.a = renderAlpha + noise * withinCircle;
}