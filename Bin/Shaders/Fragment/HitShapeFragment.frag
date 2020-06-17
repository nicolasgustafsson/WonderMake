#version 440
out vec4 FragColor;

in float uv;
in float progress;
in vec2 coord;

//!#include "Common.glsl"
uniform vec2 Size;
uniform float AnticipationProgress;
uniform float SpawnTime;
uniform float HitProgress;
uniform float Delay;

//  Function from Iñigo Quiles
//  www.iquilezles.org/www/articles/functions/functions.htm
float parabola( float x, float k ){
    return pow( 4.0*x*(1.0-x), k );
}

float mapRange(float low1, float high1, float low2, float high2, float value)
{
   return low2 + (value - low1) * (high2 - low2) / (high1 - low1);
}

float handleThickness(float thickness, float low, float high, float value)
{
    return mapRange(low - thickness, high + thickness, low, high , value);
}

float plot(vec2 st, float pct, float fade1, float fade2)
{
	fade1 *= Size.x / Size.y;
	fade2 *= Size.x / Size.y;
	
  return  (smoothstep( pct-fade1, pct, st.y) -
          smoothstep( pct, pct+fade2, st.y));
}

float plot(vec2 st, float pct, float fade)
{
	return plot(st, pct, fade, fade);
}

float plotx(vec2 st, float pct, float fade){
  return  smoothstep( pct-fade, pct, st.x) -
          smoothstep( pct, pct+fade, st.x);
}

vec3 outline(float outerBorder, float innerBorder)
{
	vec3 black = vec3(0.);
	vec3 white = vec3(1.);
	
	return black * outerBorder + white * innerBorder;
}

vec3 filling(float outerBorder, float innerBorder)
{
	vec3 grey = vec3(0.6, 0.6, 0.6);
	
	return grey;
}

vec3 anticipationThang(float outerBorder, float innerBorder, float aAnticipation)
{
	vec3 grey = vec3(0.6, 0.6, 0.6);
	
	return grey * aAnticipation;
}

vec3 hitProgressThang(float outerBorder, float innerBorder, float aHitProgress)
{
	vec3 red = vec3(1.0, 0.0, 0.0);
	
	return red * aHitProgress;
}

float lifeTime;

void main() 
{
	lifeTime = Time - (SpawnTime + Delay);
    vec2 st = vec2(uv, progress);
    
    float thickness = 0.86;
    
    float y = handleThickness(thickness, 0.8, 1.0, mapRange(0., 1., 0.5, 0.9, parabola(st.x,1.000)));
    
	float yAnticipation = mapRange(0.8, 1.0, AnticipationProgress - 0.2, AnticipationProgress, y); 
	float anticipation = min(1.0, plot(st, yAnticipation, thickness * 5.0, thickness * 0.1));
	
	float yHitProgress = mapRange(0.8, 1.0, HitProgress - 0.2, HitProgress, y); 
	float hitFade = min(1.0, plot(st, yHitProgress - 0.5, thickness * 9999.0, thickness * 10.0));
	float hitProgress = min(1.0, plot(st, yHitProgress, thickness * 9999.0, thickness * 0.2));
	
    float y2 = mapRange(0.85, 1.0, 0.0, 0.2, y);

    st.x = abs(mapRange(0.0, 1.0, -1.0, 1.0, st.x));
    
    float belowTopMul = 1.0 - smoothstep(y, y , st.y);
 	float aboveTopMul = smoothstep(y2 , y2,  st.y);
    float insideYMul = belowTopMul * aboveTopMul;
    float insideXMul = 1.0 - smoothstep(0.8 + thickness * 0.5, 0.85 + thickness * 2.0, st.x);
    
    float insideMul = insideYMul * insideXMul;

    vec3 color = vec3(1.0, 1.0, 1.0);
    float line = max(plot(st,y, thickness * 2.0), plot(st,y2, thickness * 2.));
	
    float line2 = plotx(st, y, thickness * 3.);
    
    color.g = max(line * insideXMul, line2  * insideYMul);
    
    float blackicity = color.g;
    
	blackicity = smoothstep(0.8, 1.0, blackicity);
	float whiticity = smoothstep(0.98, 1.0, blackicity);
    color = vec3(whiticity);
	
	blackicity = blackicity * insideMul;
	
    color.r = blackicity;
	
	float outerBorder = whiticity;
	float innerBorder = blackicity - outerBorder;
	float inside = (insideMul - outerBorder) - innerBorder;
	
	color = (outline(outerBorder, innerBorder) 
	+ filling(outerBorder, innerBorder) 
	+ anticipationThang(outerBorder, innerBorder, anticipation)) * (1.0 - hitProgress)
	+ hitProgressThang(outerBorder, innerBorder, hitProgress);
	
	float fadeDuration = 0.2;
	float fadeIn = smoothstep(0., fadeDuration, lifeTime);
	float fadeOut = 1.0 - hitFade;
	float fade = min(1.0, fadeIn) * min(1.0, fadeOut);
	
	float baseFadeMultiplier = 0.5 * fade;
	float anticipationFade = (anticipation * 0.5);
	float hitMultiplier = (hitProgress * 0.5);
	
	float hitOrAnticipation = max(anticipationFade, 1.0 - hitFade);
	
	float alpha = insideMul * (hitOrAnticipation + baseFadeMultiplier);
    FragColor = vec4(color, alpha);
}
