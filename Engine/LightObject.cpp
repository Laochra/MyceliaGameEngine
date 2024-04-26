#include "LightObject.h"

#include "LightingManager.h"

void LightObject::Initialise()
{
	GameObject3D::Initialise();
	LightingManager::lightObjects.push_back(this);
}

void LightObject::OnDestroy()
{
	for (int l = 0; l < LightingManager::lightObjects.size(); l++)
	{
		if (*LightingManager::lightObjects[l] == *this)
		{
			LightingManager::lightObjects.erase(LightingManager::lightObjects.begin() + l);
		}
	}
	GameObject3D::OnDestroy();
}
