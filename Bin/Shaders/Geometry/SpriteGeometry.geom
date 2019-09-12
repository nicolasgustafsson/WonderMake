#version 440 
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;
in vec2 vertexScale[];
in vec2 vertexOrigin[];
in vec4 vertexColor[];
in float vertexRotation[];

out vec2 TexCoord;
out vec4 Color;
uniform vec2 TextureSize;

layout (std140, binding = 0) uniform Engine
{
	mat3 ViewMatrix;
	mat3 ProjectionMatrix;
	mat3 ViewProjectionMatrix;
    float Time;
};


void ConstructSpriteRectangle(vec2 position, vec2 origin, vec2 scale, vec4 color, float rotation)
{    
	gl_Position.zw = vec2(0.0, 1.0);
	
	Color = vertexColor[0];
	
	float width = TextureSize.x * scale.x;
	float height = TextureSize.y * scale.y;
	
	mat2 rotationMatrix = mat2(cos(rotation), sin(rotation), -sin(rotation), cos(rotation));
	vec2 rotatedScale = vec2(-width, -height) * rotationMatrix;
	
	position += origin * vec2(width, -height) * rotationMatrix;
	
    gl_Position.xy = (ViewProjectionMatrix * vec3((position + rotatedScale.xy).xy, 1.0)).xy;    // 1:bottom-left
	
	TexCoord = vec2(0.0, 0.0);
    EmitVertex(); 
	
	rotatedScale = vec2(width, -height) * rotationMatrix;
	
    gl_Position.xy = (ViewProjectionMatrix * vec3((position + rotatedScale.xy).xy, 1.0)).xy;    // 2:bottom-right

	TexCoord = vec2(1.0, 0.0);
    EmitVertex();
	
	rotatedScale = vec2(-width, height) * rotationMatrix;
    gl_Position.xy = (ViewProjectionMatrix * vec3((position + rotatedScale.xy).xy, 1.0)).xy;     // 3:top-left
	
	TexCoord = vec2(0.0, 1.0);
    EmitVertex();
	
	rotatedScale = vec2(width, height) * rotationMatrix;
    gl_Position.xy = (ViewProjectionMatrix * vec3((position + rotatedScale.xy).xy, 1.0)).xy;     // 4:top-right
	
	TexCoord = vec2(1.0, 1.0);
    EmitVertex();
	
    EndPrimitive();
}

void main() {    
    ConstructSpriteRectangle(gl_in[0].gl_Position.xy, vertexOrigin[0], vertexScale[0], vertexColor[0], vertexRotation[0]);
}  