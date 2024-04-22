#pragma once

#include "MathIncludes.h"

#include <vector>
using std::vector;

#include "Light.h"
#include "LightObject.h"

namespace LightingManager
{
	extern Light ambientLight;

	extern DirectionalLight directionalLight;
	extern vector<LightObject*> lightObjects;

	extern vector<LightObject*> GetClosestLightObjects(const vec3 position, const int count = 4) noexcept;
}