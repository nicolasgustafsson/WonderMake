#version 440 
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

out vec2 TexCoord;
uniform vec2 ScreenSize = vec2(1600.0, 900.0);
uniform vec2 TextureSize;

void build_house(vec4 position)
{    
	float width = (TextureSize.x / ScreenSize.x);
	float height = (TextureSize.y / ScreenSize.y);
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
    build_house(gl_in[0].gl_Position);
}  