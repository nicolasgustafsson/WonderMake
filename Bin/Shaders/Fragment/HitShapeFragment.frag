#version 440
out vec4 FragColor;

in float uv;
in float progress;

layout (std140, binding = 0) uniform Engine
{
	mat3 ViewMatrix;
	mat3 ProjectionMatrix;
	mat3 ViewProjectionMatrix;
    float Time;
};

void main()
{
	float alpha = smoothstep(0.0, 0.2, uv) * smoothstep(1.0, 0.8, uv);
	
	//float red = lerp(0.f, 1.f, progress);
	FragColor = vec4(1.0,1.0, 1.0, alpha);
    //FragColor = vec4(1.0, 1.0 - red, 1.0 - red, alpha);
}