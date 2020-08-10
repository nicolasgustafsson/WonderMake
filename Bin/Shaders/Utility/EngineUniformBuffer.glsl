
layout (std140, binding = 0) uniform Engine
{
	mat3 ViewMatrix;
	mat3 ProjectionMatrix;
	mat3 ViewProjectionMatrix;
	vec2 Resolution;
	vec2 CameraPosition;
    float Time;
};
