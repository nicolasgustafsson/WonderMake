#version 440
out vec4 FragColor;
  
in vec2 TexCoords;
in vec4 Color;

uniform sampler2D ourTexture;

void main()
{
    FragColor = texture(ourTexture, TexCoords) * Color;
}