#version 440
out vec4 FragColor;
  
in vec2 TexCoord;
uniform vec4 MainColor = vec4(0.0, 1.0, 1.0, 1.0);
uniform vec4 DetailColor = vec4(1.0, 1.0, 1.0, 1.0);

//!#include "Common.glsl"

void main()
{
	float aspectRatio = Resolution.x / Resolution.y;
	
	vec2 uv = TexCoord;
	
	vec2 offset = CameraPosition / Resolution;
	offset.x *= -1.0;
	
	uv -= offset;
	uv.x *= aspectRatio;
	
	float noise1 = pow((simplex3d(vec3(uv * 100.f, Time * 0.1)) + 1.0) / 2.f, 2) * 1.5;
	
	float noise2 = pow((simplex3d(vec3(uv * 10.f, Time * 0.1)) + 1.0) / 2.f, 2);
	
	float noise = min(noise1, noise2);
	
	float labyrinth = labyrinthMoving(uv * 90.f, Time * 1);

	FragColor = mix (MainColor, DetailColor, smoothstep(0.05, 0.02, noise) * labyrinth);
	
	FragColor.a = 1.0;
}