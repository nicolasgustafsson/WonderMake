
layout (std140, binding = 0) uniform Engine
{
	mat3 ViewMatrix;
	mat3 ProjectionMatrix;
	mat3 ViewProjectionMatrix;
    float Time;
};
