#version 440 
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;
in vec2 vertexScale[];
in vec4 vertexColor[];

out vec2 TexCoord;
out vec4 Color;
uniform vec2 ScreenSize = vec2(1600.0, 900.0);
uniform vec2 TextureSize;

void ConstructSpriteRectangle(vec4 position, vec2 scale, vec4 color)
{    
	Color = vertexColor[0];
	float width = (TextureSize.x / ScreenSize.x) * scale.x;
	float height = (TextureSize.y / ScreenSize.y) * scale.y;
	
    gl_Position = position + vec4(-width, -height, 0.0, 0.0);    // 1:bottom-left
	TexCoord = vec2(0.0, 0.0);
    EmitVertex(); 
	
    gl_Position = position + vec4( width, -height, 0.0, 0.0);    // 2:bottom-right
	TexCoord = vec2(1.0, 0.0);
    EmitVertex();
	
    gl_Position = position + vec4(-width,  height, 0.0, 0.0);    // 3:top-left
	TexCoord = vec2(0.0, 1.0);
    EmitVertex();
	
    gl_Position = position + vec4( width,  height, 0.0, 0.0);    // 4:top-right
	TexCoord = vec2(1.0, 1.0);
    EmitVertex();
	
    EndPrimitive();
}

void main() {    
    ConstructSpriteRectangle(gl_in[0].gl_Position, vertexScale[0], vertexColor[0]);
}  