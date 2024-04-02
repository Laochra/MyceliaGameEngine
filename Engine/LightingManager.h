#pragma once

#include "MathIncludes.h"

struct Light
{
	vec3 colour;
};

struct DirectionalLight : Light
{
	vec3 direction;
};

struct PointLight : Light
{
	vec3 position;
	float range;
};

namespace LightingManager
{
	extern Light ambientLight;

	extern DirectionalLight directionalLight;
	extern PointLight pointLight;
}