
layout (std140, binding = 0) uniform Engine
{
    float Time;
};

layout (std140, binding = 1) uniform Camera
{
	mat3 ViewMatrix;
	vec2 CameraPosition;
};

layout (std140, binding = 2) uniform Display
{
	mat3 ProjectionMatrix;
	mat3 ViewProjectionMatrix;
	vec2 Resolution;
	vec2 InverseResolution;
};