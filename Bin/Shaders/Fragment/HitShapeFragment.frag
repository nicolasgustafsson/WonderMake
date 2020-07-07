#version 440
out vec4 FragColor;

in float uv;
in float progress;
in vec2 coord;

//!#include "Common.glsl"
//!#include "SimplexNoise.glsl"

uniform vec2 Size;
uniform float AnticipationProgress;
uniform float SpawnTime;
uniform float HitProgress;
uniform float Delay;

float lifeTime;
vec2 uvCoordinates;

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

float calculateInsideMultiplier(float y, float y2, vec2 st, float thickness)
{
    float belowTopMul = 1.0 - smoothstep(y, y , st.y);
 	float aboveTopMul = smoothstep(y2 , y2,  st.y);
    float insideYMul = belowTopMul * aboveTopMul;
    float insideXMul = 1.0 - smoothstep(0.8 + thickness * 0.5, 0.85 + thickness * 2.0, st.x);
    
    float insideMul = insideYMul * insideXMul;

	return insideMul;
}

float plot(vec2 curvePosition, float pct, float fade1, float fade2)
{
	fade1 *= Size.x / Size.y;
	fade2 *= Size.x / Size.y;
	
  return  (smoothstep( pct-fade1, pct, curvePosition.y) -
          smoothstep( pct, pct+fade2, curvePosition.y));
}

float plot(vec2 curvePosition, float pct, float fade)
{
	return plot(curvePosition, pct, fade, fade);
}

float plotx(vec2 curvePosition, float pct, float fade){
  return  smoothstep( pct-fade, pct, curvePosition.x) -
          smoothstep( pct, pct+fade, curvePosition.x);
}

vec3 outline(float outerBorder, float innerBorder)
{
	vec3 black = vec3(0.2);
	vec3 white = vec3(1.);
	vec3 grey = vec3(0.5, 0.5, 0.5);
	
	return black * outerBorder + grey * innerBorder;
}

vec3 filling(float outerBorder, float innerBorder)
{
	vec3 grey = vec3(0.5, 0.5, 0.5);
	
	return grey;
}

vec3 anticipationThang(float outerBorder, float innerBorder, float aAnticipation)
{
	vec3 grey = vec3(0.2, 0.2, 0.2);
	
	return grey * aAnticipation;
}

vec3 hitProgressThang(float outerBorder, float innerBorder, float aHitProgress, float noise)
{
	vec3 red = vec3(0.6, 0.0, 0.0);

	return ((red + vec3(0.6, 0.6, 0.) * noise) - outerBorder * vec3(0.2, 0.2, 0.2)) * max(0, aHitProgress);
}

void main() 
{

	lifeTime = Time - (SpawnTime);

    uvCoordinates = vec2(uv, progress);


    float thickness = 0.86;
    
	// maps x(progress along bezier) to a parabola; not sure why it's 0.5 - 0.8 range
	float closenessToCurve = mapRange(0., 1., 0.5, 0.8, parabola(uvCoordinates.x, 1.00)); 
    closenessToCurve = handleThickness(thickness, 0.8, 1.0, closenessToCurve);
    
    float noise = abs(simplex3d(vec3(uvCoordinates.x * 15., uvCoordinates.y * 2. - Time * 1., 0.)));


	float yAnticipation = mapRange(0.8, 1.0, AnticipationProgress - 0.2, AnticipationProgress, closenessToCurve); 
	float anticipation = min(1.0, plot(uvCoordinates, yAnticipation, thickness * 5.0, thickness * 0.1));
	
	float yHitProgress = mapRange(0.8, 1.0, HitProgress - 0.2, HitProgress, closenessToCurve); 
	
	float hitProgress = min(1.0, plot(uvCoordinates, yHitProgress, thickness * 9999.0, thickness * 0.2));
	float hitFade = min(1.0, plot(uvCoordinates, yHitProgress - 1.5, thickness * 9999.0, thickness * 1.)) * hitProgress;
    float closenessToStart = mapRange(0.85, 1.0, 0., 0.2, closenessToCurve) * 1.f;

    uvCoordinates.x = abs(mapRange(0.0, 1.0, -1.0, 1.0, uvCoordinates.x));
   
    float insideMul = calculateInsideMultiplier(closenessToCurve, closenessToStart, uvCoordinates, thickness);

    vec3 color = vec3(1.0, 1.0, 1.0);
    float line = max(plot(uvCoordinates,closenessToCurve, thickness * 2.0), plot(uvCoordinates,closenessToStart, thickness * 2.));
	
    float line2 = plotx(uvCoordinates, closenessToCurve, thickness * 3.);
    
    float blackicity = max(line * insideMul, line2  * insideMul);
    
	blackicity = smoothstep(0.8, 1.0, blackicity);

	float outerBorder = smoothstep(0.98, 1.0, blackicity);
	
	float innerBorder = blackicity - outerBorder;
	float inside = (insideMul - outerBorder) - innerBorder;
	
	color = (outline(outerBorder, innerBorder) 
	+ filling(outerBorder, innerBorder) 
	+ anticipationThang(outerBorder, innerBorder, anticipation)) * (1.0 - hitProgress)
	+ hitProgressThang(outerBorder, innerBorder, hitProgress, noise)
	+ noise * 0.5;

	float fadeDuration = 0.2;
	float fadeIn = smoothstep(0., fadeDuration, lifeTime);
	float fadeOut = 1.0 - hitFade;
	float fade = min(1.0, fadeIn) * min(1.0, fadeOut);
	
	float baseFadeMultiplier = 0.5 * fade;
	float anticipationFade = (anticipation * 0.5);
	float hitMultiplier = (hitProgress * 0.5);
	
	float hitOrAnticipation = max(anticipationFade, hitMultiplier) * fadeOut;

	float alpha = insideMul * (hitOrAnticipation + baseFadeMultiplier);
    FragColor = vec4(color, alpha);
}
