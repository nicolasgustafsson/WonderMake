//!#include "../Common.glsl"

vec2 CorrectSubpixelPositioning(vec2 aPosition)
{
	aPosition *= Resolution;
	
	aPosition = round(aPosition);
	
	return aPosition * InverseResolution;
}