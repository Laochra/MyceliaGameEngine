#include "LightingManager.h"

namespace LightingManager
{
	Light ambientLight = Light();

	DirectionalLight directionalLight = DirectionalLight();
	vector<LightObject*> lightObjects;

	vector<LightObject*> GetClosestLightObjects(const vec3 position, const int count) noexcept
	{
		vector<LightObject*> result;

		for (int pl = 0; pl < lightObjects.size(); pl++)
		{
			// If count hasn't been reached, just add it
			if (pl < count)
			{
				result.push_back(lightObjects[pl]);
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
			if (glm::length2(lightObjects[pl]->GetPosition() - position) < glm::length2(result[furthestIndex]->GetPosition() - position))
			{
				result[furthestIndex] = lightObjects[pl];
			}
		}

		return result;
	}
}