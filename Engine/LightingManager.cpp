#include "LightingManager.h"

namespace LightingManager
{
	Light ambientLight = Light();

	DirectionalLight directionalLight = DirectionalLight();
	vector<LightObject*> lightObjects;

	vector<LightObject*> GetClosestLightObjects(const vec3 position, const int count) noexcept
	{
		vector<LightObject*> result;

		for (int l = 0; l < lightObjects.size(); l++)
		{
			if (lightObjects[l] != GameObject::Active) continue; // Skip inactive lights

			// If count hasn't been reached, just add it
			if (l < count)
			{
				result.push_back(lightObjects[l]);
				continue;
			}

			// Find furthest current result
			int furthestIndex = 0;
			for (int r = 1; r < result.size(); r++)
			{
				if (glm::length2(result[r]->GetPosition() - position) > glm::length2(result[furthestIndex]->GetPosition() - position))
				{
					furthestIndex = r;
				}
			}
			// Swap with furthest current result if it would mean a closer light
			if (glm::length2(lightObjects[l]->GetPosition() - position) < glm::length2(result[furthestIndex]->GetPosition() - position))
			{
				result[furthestIndex] = lightObjects[l];
			}
		}

		return result;
	}
}