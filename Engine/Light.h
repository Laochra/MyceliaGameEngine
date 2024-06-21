#pragma once

#include "MathIncludes.h"

typedef unsigned int uint;

class Light
{
public:
	vec3 colour;
	float intensity;
	vector<uint> shadowMaps;

	Light(vec3 colourInit = vec3(1.0f)) : colour(colourInit) {}
};

class DirectionalLight : public Light
{
public:
	vec3 direction;

	DirectionalLight(vec3 colourInit = vec3(1.0f), vec3 directionInit = vec3(0.0f, -1.0f, 0.0f)) :
		Light(colourInit),
		direction(directionInit) {}
};