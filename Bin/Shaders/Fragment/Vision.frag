#version 440  
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D ourTexture;

//!#include "Common.glsl"

void main()
{
    FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}