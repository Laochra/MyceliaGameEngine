#pragma once

#include "MathIncludes.h"

#include <vector>
using std::vector;

struct Light
{
	vec3 colour;
	
	Light(vec3 colourInit = vec3(0.0f)) : colour(colourInit) {}
};

struct DirectionalLight : Light
{
	vec3 direction;

	DirectionalLight(vec3 colourInit = vec3(0.0f), vec3 directionInit = vec3(0.0f, -1.0f, 0.0f)) :
		Light(colourInit),
		direction(directionInit) {}
};

struct PointLight : Light
{
	vec3 position;
	float range;

	PointLight(vec3 colourInit = vec3(0.0f), vec3 posInit = vec3(0.0f), float rangeInit = 0.0f) :
		Light(colourInit),
		position(posInit),
		range(rangeInit) {}
};

namespace LightingManager
{
	extern Light ambientLight;

	extern DirectionalLight directionalLight;
	extern vector<PointLight> pointLights;

	extern vector<PointLight> GetClosestPointLights(const vec3 position, const int count = 4) noexcept;
}