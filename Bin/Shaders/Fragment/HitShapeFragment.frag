#version 440
out vec4 FragColor;

in float uv;
in float progress;
in vec2 coord;

layout (std140, binding = 0) uniform Engine
{
	mat3 ViewMatrix;
	mat3 ProjectionMatrix;
	mat3 ViewProjectionMatrix;
    float Time;
};
uniform vec2 Size;
uniform float AnticipationProgress;
uniform float SpawnTime;
uniform float HitProgress;

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

float plot(vec2 st, float pct, float fade){
	fade *= Size.x / Size.y;
	
  return  (smoothstep( pct-fade, pct, st.y) -
          smoothstep( pct, pct+fade, st.y));
}

float plotx(vec2 st, float pct, float fade){
  return  smoothstep( pct-fade, pct, st.x) -
          smoothstep( pct, pct+fade, st.x);
}

vec3 outline(float outerBorder, float innerBorder, float inside)
{
	vec3 black = vec3(0.);
	vec3 white = vec3(1.);
	
	return black * outerBorder + white * innerBorder;
}

vec3 filling(float outerBorder, float innerBorder, float inside)
{
	vec3 grey = vec3(0.6, 0.6, 0.6);
	
	return grey * inside;
}

vec3 anticipationThang(float outerBorder, float innerBorder, float inside, float aAnticipation)
{
	vec3 grey = vec3(0.6, 0.6, 0.6);
	
	return grey * inside * aAnticipation;
}

float lifeTime;

void main() {

	lifeTime = Time - SpawnTime;
    vec2 st = vec2(uv, progress);
    
    float thickness = 0.86;
    
    float y = handleThickness(thickness, 0.8, 1.0, mapRange(0., 1., 0.5, 0.9, parabola(st.x,1.000)));
    
	float yAnticipation = mapRange(0.8, 1.0, AnticipationProgress - 0.2, AnticipationProgress, y);
	 
    float y2 = mapRange(0.85, 1.0, 0.0, 0.2, y);

    st.x = abs(mapRange(0.0, 1.0, -1.0, 1.0, st.x));
    
    float belowTopMul = 1.0 - smoothstep(y, y , st.y);
 	float aboveTopMul = smoothstep(y2 , y2,  st.y);
    float insideYMul = belowTopMul * aboveTopMul;
    float insideXMul = 1.0 - smoothstep(0.8 + thickness * 0.5, 0.85 + thickness * 2.0, st.x);
    
    float insideMul = insideYMul * insideXMul;

    vec3 color = vec3(1.0, 1.0, 1.0);
    float line = max(plot(st,y, thickness * 2.), plot(st,y2, thickness * 2.));
	
	float anticipation = plot(st, yAnticipation, thickness * 2.);
	
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
	
	color = outline(outerBorder, innerBorder, inside) + filling(outerBorder, innerBorder, inside) + anticipationThang(outerBorder, innerBorder, inside, anticipation);
    FragColor = vec4(color,insideMul * 0.2 * smoothstep(0., 0.1, lifeTime) + anticipation * 0.5 * insideMul);
}
