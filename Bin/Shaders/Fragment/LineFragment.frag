#version 440
out vec4 FragColor;
  
in vec2 TexCoord;
in vec4 vertexColor;

void main()
{
    FragColor = vertexColor;
}