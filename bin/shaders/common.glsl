//!#include "engine_uniform_buffer.glsl"

//makes the position (100.35, 53.86) to (100, 54) -> this fixes some aliasing issues when moving the camera
vec2 CorrectSubpixelPositioning(vec2 aPosition)
{
	aPosition *= Resolution;
	
	aPosition = round(aPosition);
	
	return aPosition * InverseResolution;
}

mat3 CreateRotationAroundX(float aRadians)
{
	return mat3(
			1, 0, 0,
			0, cos(aRadians), -sin(aRadians),
			0, sin(aRadians), cos(aRadians));
}
mat3 CreateRotationAroundY(float aRadians)
{
	return mat3(
			cos(aRadians), 0, sin(aRadians),
			0, 1, 0,
			-sin(aRadians), 0, cos(aRadians));
}
mat3 CreateRotationAroundZ(float aRadians)
{
	return mat3(
			cos(aRadians), -sin(aRadians), 0,
			sin(aRadians), cos(aRadians), 0, 
			0, 0, 1);
}
mat2 CreateRotation2D(float aRadians)
{
	return mat2(
		cos(aRadians), -sin(aRadians),
		sin(aRadians), cos(aRadians));
}

float Gaussian(float x, float mean, float stddev)
{
    return exp(-((x - mean) * (x - mean)) / (2.0 * stddev * stddev));
}
