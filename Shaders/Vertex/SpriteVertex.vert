#version 440
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aScale;
layout (location = 2) in vec4 aColor;

out vec2 vertexScale;
out vec4 vertexColor;

void main() 
{
  gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
  
  vertexScale = aScale;
  vertexColor = aColor;
};