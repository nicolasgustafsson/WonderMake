#version 440
out vec4 FragColor;
  
in vec2 TexCoord;
in vec4 Color;

uniform sampler2D ourTexture;

layout (std140, binding = 0) uniform Engine
{
	mat3 ViewMatrix;
	mat3 ProjectionMatrix;
	mat3 ViewProjectionMatrix;
    float Time;
};

void main()
{
   float dist = distance(TexCoord, vec2(0.5, 0.5)) * 2.0;
	
    float withinCircle = 1.0 - smoothstep(dist + 0.01f, dist, 1.0);
    
    float renderAlpha = min(withinCircle, dist);

    // Output to screen
    FragColor = Color * renderAlpha;
}