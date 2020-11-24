#version 440
layout (location = 0) in vec2 aPos;

//!#include "Common.glsl"

void main() 
{
	vec3 transformedPosition = ViewProjectionMatrix * vec3(aPos.x, aPos.y, 1.0);

	gl_Position.xy = CorrectSubpixelPositioning(transformedPosition.xy);

	gl_Position.z = 0.0;
	gl_Position.w = 1.0;
};