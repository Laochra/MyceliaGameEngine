#pragma once

#include "MathIncludes.h"

typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char ubyte;

enum ShadowMode : ubyte { NoShadows, HardShadows, SoftShadows };

class Light
{
public:
	vec3 colour;
	float intensity = 1.0f; 
	
	Light(vec3 colourInit = vec3(1.0f)) : colour(colourInit) {}
};

class DirectionalLight : public Light
{
public:
	vec3 direction;
	ShadowMode shadowMode = HardShadows;
	uint shadowMap = 0U;

	DirectionalLight(vec3 colourInit = vec3(1.0f), vec3 directionInit = vec3(0.0f, -1.0f, 0.0f)) :
		Light(colourInit),
		direction(directionInit) {}
};