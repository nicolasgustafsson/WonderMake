#version 440 
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;
in vec2 vertexScale[];
in vec4 vertexColor[];
in float vertexRotation[];

out vec2 TexCoord;
out vec4 Color;
uniform vec2 ScreenSize = vec2(1600.0, 900.0);
uniform vec2 TextureSize;

void ConstructSpriteRectangle(vec2 position, vec2 scale, vec4 color, float rotation)
{    
	Color = vertexColor[0];
	
	float width = (TextureSize.x) * scale.x;
	float height = (TextureSize.y) * scale.y;
	
	mat2 rotationMatrix = mat2(cos(rotation), sin(rotation), -sin(rotation), cos(rotation));
	vec2 rotatedScale = vec2(-width, height) * rotationMatrix;
	
    gl_Position.xy = position + rotatedScale / ScreenSize;    // 1:bottom-left
	
	TexCoord = vec2(0.0, 0.0);
    EmitVertex(); 
	
	rotatedScale = vec2(width, height) * rotationMatrix;
	
    gl_Position.xy = position + rotatedScale / ScreenSize;    // 2:bottom-right

	TexCoord = vec2(1.0, 0.0);
    EmitVertex();
	
	rotatedScale = vec2(-width, -height) * rotationMatrix;
    gl_Position.xy = position + rotatedScale / ScreenSize;     // 3:top-left
	
	TexCoord = vec2(0.0, 1.0);
    EmitVertex();
	
	rotatedScale = vec2(width, -height) * rotationMatrix;
    gl_Position.xy = position + rotatedScale / ScreenSize;     // 4:top-right
	
	TexCoord = vec2(1.0, 1.0);
    EmitVertex();
	
    EndPrimitive();
}

void main() {    
    ConstructSpriteRectangle(gl_in[0].gl_Position.xy, vertexScale[0], vertexColor[0], vertexRotation[0]);
}  