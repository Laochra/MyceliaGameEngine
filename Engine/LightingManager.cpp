#include "LightingManager.h"

namespace LightingManager
{
	Light ambientLight = Light();

	DirectionalLight directionalLight = DirectionalLight();
	vector<PointLight> pointLights;

	vector<PointLight> GetClosestPointLights(const vec3 position, const int count) noexcept
	{
		vector<PointLight> result;

		for (int pl = 0; pl < pointLights.size(); pl++)
		{
			// If count hasn't been reached, just add it
			if (pl < count)
			{
				result.push_back(pointLights[pl]);
				continue;
			}

			// Find furthest current result
			int furthestIndex = 0;
			for (int r = 1; r < result.size(); r++)
			{
				if (glm::length2(result[r].position - position) > glm::length2(result[furthestIndex].position - position))
				{
					furthestIndex = r;
				}
			}
			// Swap with furthest current result if it would mean a closer light
			if (glm::length2(pointLights[pl].position - position) < glm::length2(result[furthestIndex].position - position))
			{
				result[furthestIndex] = pointLights[pl];
			}
		}

		// Ensure at least as many lights as were requested are returned
		while (result.size() < count)
		{
			result.push_back(PointLight());
		}

		return result;
	}
}