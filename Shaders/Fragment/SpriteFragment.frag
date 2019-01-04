#version 440
out vec4 FragColor;
  
in vec2 TexCoord;
in vec4 Color;

uniform sampler2D ourTexture;

layout (std140, binding = 0) uniform Engine
{
    float Time;
};

void main()
{
    FragColor = texture(ourTexture, TexCoord) * Color;
	
	FragColor.g = sin(Time);
}