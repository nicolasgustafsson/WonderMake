//!#include "../common.glsl"

//makes the position (100.35, 53.86) to (100, 54) -> this fixes some aliasing issues when moving the camera
vec2 CorrectSubpixelPositioning(vec2 aPosition)
{
	aPosition *= Resolution;
	
	aPosition = round(aPosition);
	
	return aPosition * InverseResolution;
}