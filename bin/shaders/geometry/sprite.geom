#version 440 
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

uniform vec2 TextureSize;

in vec2 vertexScale[];
in vec2 vertexOrigin[];
in vec4 vertexColor[];
in float vertexRotation[];

out vec2 TexCoord;
out vec4 Color;

//!#include "common.glsl"

void ConstructSpriteRectangle(vec2 position, vec2 origin, vec2 scale, vec4 color, float rotation)
{    
	gl_Position.zw = vec2(0.0, 1.0);
	Color = vertexColor[0];

	float width = TextureSize.x * scale.x * 0.5;
	float height = TextureSize.y * scale.y * 0.5;
	mat2 rotationMatrix = mat2(cos(rotation), -sin(rotation), sin(rotation), cos(rotation));

	position += origin * vec2(width, height) * rotationMatrix;

	// 1:bottom-left

	vec2 rotatedScale = vec2(-width, -height) * rotationMatrix;

	gl_Position.xy = (vec3((position + rotatedScale.xy).xy, 1.0) * ViewProjectionMatrix).xy;
	TexCoord = vec2(0.0, 1.0);

	EmitVertex(); 

	// 2:bottom-right

	rotatedScale = vec2(width, -height) * rotationMatrix;

	gl_Position.xy = (vec3((position + rotatedScale.xy).xy, 1.0) * ViewProjectionMatrix).xy;
	TexCoord = vec2(1.0, 1.0);

	EmitVertex();

	// 3:top-left

	rotatedScale = vec2(-width, height) * rotationMatrix;

	gl_Position.xy = (vec3((position + rotatedScale.xy).xy, 1.0) * ViewProjectionMatrix).xy;
	TexCoord = vec2(0.0, 0.0);

	EmitVertex();

	// 4:top-right

	rotatedScale = vec2(width, height) * rotationMatrix;

	gl_Position.xy = (vec3((position + rotatedScale.xy).xy, 1.0) * ViewProjectionMatrix).xy;
	TexCoord = vec2(1.0, 0.0);

	EmitVertex();

	EndPrimitive();
}

void main()
{    
	ConstructSpriteRectangle(gl_in[0].gl_Position.xy, vertexOrigin[0], vertexScale[0], vertexColor[0], vertexRotation[0]);
}