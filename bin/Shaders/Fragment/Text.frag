#version 440  
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D ourTexture;

//!#include "Common.glsl"

uniform vec4 Color = vec4(0.0, 0.0, 0.0, 0.0);
uniform vec4 fgColor = vec4(0.0, 0.0, 0.0, 1.0);

uniform float Size;

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

void main() {
    vec3 msd = texture(ourTexture, TexCoord).rgb;
    float sd = median(msd.r, msd.g, msd.b);
    float screenPxDistance = (Size / 12.0 + 1.0)*(sd - 0.5);
    float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);
    FragColor = Color; //mix(bgColor, fgColor, opacity);
	
	FragColor.a = opacity;
}

