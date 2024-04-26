#version 460

uniform sampler2D AliasedTexture;

in vec2 FragTexCoords;

out vec4 FragColour;

#define spanmax 8.0
#define reducemul (1.0 / spanmax)
#define reducemin (1.0 / 128.0)

void main() // Fragment
{
	vec2 resolution = textureSize(AliasedTexture, 0);
	vec2 texelSize = 1.0 / resolution;
	
	vec3 colourC =  texture(AliasedTexture, FragTexCoords).rgb;
	vec3 colourNW = texture(AliasedTexture, FragTexCoords + vec2(-texelSize.x,  texelSize.y)).rgb;
	vec3 colourNE = texture(AliasedTexture, FragTexCoords + vec2( texelSize.x,  texelSize.y)).rgb;
	vec3 colourSW = texture(AliasedTexture, FragTexCoords + vec2(-texelSize.x, -texelSize.y)).rgb;
	vec3 colourSE = texture(AliasedTexture, FragTexCoords + vec2( texelSize.x, -texelSize.y)).rgb;
	
	const vec3 brightnessConstant = vec3(0.299, 0.587, 0.114);
	float brightnessC =  dot(colourC,  brightnessConstant);
	float brightnessNW = dot(colourNW, brightnessConstant);
	float brightnessNE = dot(colourNE, brightnessConstant);
	float brightnessSW = dot(colourSW, brightnessConstant);
	float brightnessSE = dot(colourSE, brightnessConstant);
	float minBrightness = min
	(
		min(brightnessNW, brightnessNE),
		min(brightnessSW, brightnessSE)
	);
	float maxBrightness = max
	(
		max(brightnessNW, brightnessNE),
		max(brightnessSW, brightnessSE)
	);
	
	vec2 dir = vec2
	(
		-((brightnessNW + brightnessNE) - (brightnessSW + brightnessSE)),
		 ((brightnessNW + brightnessSW) - (brightnessNE + brightnessSE))
	);
	float dirReduce = max
	(
		(brightnessNW + brightnessNE + brightnessSW + brightnessSE) * (0.25 * reducemul),
		reducemin
	);
	float dirMin = 1.0 /
	(
		min(abs(dir.x),
		abs(dir.y)) + dirReduce
	);
	vec2 newDir = min(vec2(spanmax), max(vec2(-spanmax), dir * dirMin)) * texelSize;
	
	vec3 colour1 = 0.5 *
	(
		texture(AliasedTexture, FragTexCoords + (newDir * (1.0 / 3.0 - 0.5))).xyz +
		texture(AliasedTexture, FragTexCoords + (newDir * (2.0 / 3.0 - 0.5))).xyz
	);
	vec3 colour2 = colour1 * (0.5 + 0.25) *
	(
		texture(AliasedTexture, FragTexCoords + (newDir * -0.5)).xyz +
		texture(AliasedTexture, FragTexCoords + (newDir *  0.5)).xyz
	);
	
	float calcedBrightness = dot(colour2, brightnessConstant);
	if ((calcedBrightness < minBrightness) || (calcedBrightness > maxBrightness))
	{
		FragColour = vec4(colour1, 1.0);
	}
	else
	{
		FragColour = vec4(colour2, 1.0);
	}
	
	//FragColour(vec3(brightnessC), 1.0);
}