#version 440
layout (location = 0) in vec2 aPos;
layout (location = 1) in float aUV;
layout (location = 2) in float aProgress;


out float uv;
out float progress;
out vec2 coord;

layout (std140, binding = 0) uniform Engine
{
	mat3 ViewMatrix;
	mat3 ProjectionMatrix;
	mat3 ViewProjectionMatrix;
    float Time;
};

void main() 
{
  vec3 transformedPosition = ViewProjectionMatrix * vec3(aPos.x, aPos.y, 1.0);
  
  gl_Position.xy = transformedPosition.xy;

  gl_Position.z = -1.0;
  gl_Position.w = 1.0;
  
  uv = aUV;
  progress = aProgress;
  
  coord = transformedPosition.xy;
};