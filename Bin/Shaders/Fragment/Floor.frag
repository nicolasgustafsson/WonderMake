#version 440
out vec4 FragColor;
  
in vec2 worldPos;
in vec4 vertexColor;


float rand2dTo1d(vec2 value, vec2 dotDir = vec2(12.9898, 78.233)){
    vec2 smallValue = sin(value);
    float random = dot(smallValue, dotDir);
    random = fract(sin(random) * 143758.5453);
    return random;
}

vec2 rand2dTo2d(vec2 value){
    return vec2(
        rand2dTo1d(value, vec2(12.989, 78.233)),
        rand2dTo1d(value, vec2(39.346, 11.135))
    );
}

float voronoiNoise(vec2 value){
    vec2 baseCell = floor(value);

    float minDistToCell = 10;
    for(int x=-1; x<=1; x++){
        for(int y=-1; y<=1; y++){
            vec2 cell = baseCell + vec2(x, y);
            vec2 cellPosition = cell + rand2dTo2d(cell);
            vec2 toCell = cellPosition - value;
            float distToCell = length(toCell);
            if(distToCell < minDistToCell){
                minDistToCell = distToCell;
            }
        }
    }
    return minDistToCell;
}


//!#include "Common.glsl"

void main()
{
    float aspectRatio = Resolution.x / Resolution.y;
	
	vec2 uv = worldPos.xy;
	
	float noise = smoothstep(-0.3, 1.0, pow(voronoiNoise(uv * 0.009) + 0.01, 4.0));
 
	float labyrinth = labyrinth(uv * 0.1);
	 
	FragColor =vertexColor;//vec4(noise, noise, noise, 1.0); 
	
	vec3 addColor = vec3(labyrinth, labyrinth, labyrinth);
	
	FragColor.rgb += addColor * noise * 0.1;
}