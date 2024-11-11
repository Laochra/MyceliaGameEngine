#include "LightingManager.h"

#include "GLIncludes.h"

#include "Debug.h"

#include "HexGameInfo.h"

namespace LightingManager
{
	Light ambientLight = Light();

	DirectionalLight directionalLight = DirectionalLight();
	vector<LightObject*> lightObjects;

	uint shadowMapFBO = 0U;
	uint shadowMaps = 0U;
	uint shadowMapSideLength = 4096U * RES_FACTOR;
	uint shadowMapCount = 0U;
	uint maxShadowMaps = 1U;

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
	void DrawToShadowMaps(uint& framebuffer, uint& shadowMaps, uint& mapCount, uint maxShadowMaps, uint shadowSideLength, vector<LightObject*>& lightObjects)
	{

		if (framebuffer == 0)
		{
			glGenFramebuffers(1, &framebuffer);
			glGenTextures(1, &shadowMaps);
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
			glBindTexture(GL_TEXTURE_2D_ARRAY, shadowMaps);
			glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_DEPTH24_STENCIL8, shadowSideLength, shadowSideLength, maxShadowMaps);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glBindTexture(GL_TEXTURE_2D_ARRAY, shadowMaps);
		mapCount = 0U;
		for (uint l = 0; l < (uint)lightObjects.size(); l++)
		{
			mapCount += lightObjects[l]->shadowMapCount;
		}

		if (mapCount > 0)
		{
			if (mapCount > maxShadowMaps)
			{
				Debug::LogWarning("There are more shadow maps (", mapCount, ") "
					"than are allowed (", maxShadowMaps, "). "
					"Reduce the amount of shadow mapped lights.");
			}

			uint mapIndex = 0;
			for (uint l = 0; l < (uint)lightObjects.size() && l < maxShadowMaps; l++)
			{
				LightObject& lightObject = *lightObjects[l];
				if (lightObject.shadowMode == NoShadows) continue;

				vector<mat4> pvMatrices = lightObject.GetLightPVMatrices();

				for (uint m = 0; m < lightObject.shadowMapCount; m++)
				{
					lightObject.shadowMaps[m] = mapIndex;

					glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
					glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowMaps, 0, mapIndex);
					glDrawBuffer(GL_NONE);
					glReadBuffer(GL_NONE);
					glBindFramebuffer(GL_FRAMEBUFFER, 0);

					glViewport(0, 0, shadowSideLength, shadowSideLength);
					glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
					glClear(GL_DEPTH_BUFFER_BIT);

					Updater::CallDrawDepth(pvMatrices[m]);

					glBindFramebuffer(GL_FRAMEBUFFER, 0);

					mapIndex++;
				}
			}
		}

	}
}