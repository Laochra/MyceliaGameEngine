#include "LightObject.h"

#include "LightingManager.h"

#include "Debug.h"

void LightObject::SerialiseTo(json& jsonObj) const
{
	GameObject3D::SerialiseTo(jsonObj);

	jsonObj["Range"] = range;

	vector<float> angleData(2);
	memcpy(angleData.data(), &angle[0], 2 * sizeof(float));
	jsonObj["Angle"] = angleData;

	vector<float> hdrColourData(4);
	memcpy(hdrColourData.data(), &colour[0], 4 * sizeof(float));
	jsonObj["HDRColour"] = hdrColourData;
}
void LightObject::DeserialiseFrom(const json& jsonObj)
{
	GameObject3D::DeserialiseFrom(jsonObj);

	range = jsonObj["Range"];

	vector<float> angleData = jsonObj["Angle"];
	memcpy(&angle[0], angleData.data(), 2 * sizeof(float));

	vector<float> hdrColourData = jsonObj["HDRColour"];
	memcpy(&colour[0], hdrColourData.data(), 4 * sizeof(float));
}

void LightObject::Draw()
{
	Colour debugColour = Colour(colour.x, colour.y, colour.z);

	if (angle[1] == 1.0f) // Check for no angle. 1.0 is the cosine of 0 degrees
	{
		debug->lines.AddSphere(GetGlobalPosition(), range, 32, debugColour);
	}
	else
	{
		vec3 direction = (vec3)glm::normalize(glm::inverse(GetMatrix())[2]);

		float outerBaseRadius = range * tan(acos(angle[1]));
		debug->lines.AddCone(GetGlobalPosition(), direction, range, outerBaseRadius, 32, debugColour);

		if (angle[0] != 1.0f && angle[0] != angle[1]) // Check for no or same angle. 1.0 is the cosine of 0 degrees
		{ 
			float innerBaseRadius = range * tan(acos(angle[0]));
			debug->lines.AddCone(GetGlobalPosition(), direction, range, innerBaseRadius, 32, debugColour);
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
