#pragma once

#include "MathIncludes.h"

#include <vector>
using std::vector;

#include "Light.h"
#include "LightObject.h"

typedef unsigned int uint;

namespace LightingManager
{
	extern Light ambientLight;

	extern DirectionalLight directionalLight;
	extern vector<LightObject*> lightObjects;

	extern uint shadowMapFBO;
	extern uint shadowMaps;
	extern uint shadowMapSideLength;
	extern uint shadowMapCount;
	extern uint maxShadowMaps;

	extern vector<LightObject*> GetClosestLightObjects(const vec3 position, const int count = 4) noexcept;

	void DrawToShadowMaps(
		uint& framebuffer = LightingManager::shadowMapFBO,
		uint& shadowMaps = LightingManager::shadowMaps,
		uint& mapCount = LightingManager::shadowMapCount,
		uint maxShadowMaps = LightingManager::maxShadowMaps,
		uint shadowSideLength = LightingManager::shadowMapSideLength,
		vector<LightObject*>& lightObjects = LightingManager::lightObjects);
}