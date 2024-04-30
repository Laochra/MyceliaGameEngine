#include "LightObject.h"

#include "LightingManager.h"

#include "Debug.h"

void LightObject::Draw()
{
	Colour debugColour = Colour(colour.x, colour.y, colour.z);

	if (angle[1] == 1.0f) // Check for no angle. 1.0 is the cosine of 0 degrees
	{
		// debug->lines.AddSphere(GetGlobalPosition(), range, debugColour);
	}
	else
	{
		vec3 direction = (vec3)glm::normalize(glm::inverse(GetMatrix())[2]);

		float outerBaseRadius = range * tan(acos(angle[1]));
		debug->lines.AddCone(GetGlobalPosition(), direction, range, outerBaseRadius, 8, debugColour);

		if (angle[0] != 1.0f && angle[0] != angle[1])
		{
			float innerBaseRadius = range * tan(acos(angle[0]));
			debug->lines.AddCone(GetGlobalPosition(), direction, range, innerBaseRadius, 8, debugColour);
		}
	}
}

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
