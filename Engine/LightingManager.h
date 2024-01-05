#pragma once

#include "MathIncludes.h"

struct Light
{
	vec3 direction;
	vec3 colour;
};

namespace LightingManager
{
	extern Light light;
	extern vec3 ambientLight;
}