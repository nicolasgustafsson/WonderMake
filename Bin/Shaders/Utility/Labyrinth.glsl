
//!#include "SimplexNoise.glsl"

mat2 lab_rot(float a){
	return mat2(cos(a),-sin(a),sin(a),cos(a));
}
float lab_hash(vec2 p, float seed){
	p = fract(p*vec2(1253.4,567.8));	
    p+= dot(p,p+10. + seed);
    return fract(p.x*p.y);
}

float labyrinth(vec2 uv, float seed)
{
    float pi = 3.14159;

 	uv*=lab_rot(45.*pi/180.);

    vec2 id = floor(uv);
	uv = fract(uv)-0.5;
 
    float rnd = lab_hash(id, seed);
    if(rnd <0.5){
    	uv.x*=-1.;
    }
    float thickness = (Resolution.x/Resolution.y)*0.12;
    
    float isWall = min(step(thickness,abs(vec3(uv.x+uv.y)-0.5)),
                   step(thickness,abs(vec3(uv.x+uv.y)+0.5))).x;  
    
    return isWall;
}


float labyrinth(vec2 uv)
{
	return labyrinth(uv, 0);
}

float labyrinthMoving(vec2 uv, float time)
{
	float noise = (simplex3d(vec3(uv, Time)) + 1.0) * 0.5;

	float from = labyrinth(uv, floor(time) - 1.);
	float to = labyrinth(uv, floor(time));
	
	return mix(to, from, step(fract(time), noise));
	
}
