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

	jsonObj["ShadowMode"] = (uint)shadowMode;
}
void LightObject::DeserialiseFrom(const json& jsonObj, GuidGeneration guidOptions)
{
	GameObject3D::DeserialiseFrom(jsonObj, guidOptions);

	range = jsonObj["Range"];

	vector<float> angleData = jsonObj["Angle"];
	memcpy(&angle[0], angleData.data(), 2 * sizeof(float));
	
	vector<float> hdrColourData = jsonObj["HDRColour"];
	memcpy(&colour[0], hdrColourData.data(), 4 * sizeof(float));

	uint shadowModeData = jsonObj["ShadowMode"];
	shadowMode = (ShadowMode)shadowModeData;

	if (shadowMode == NoShadows) shadowMapCount = 0U;
	else if(angle[1] != 1.0f) shadowMapCount = 1U;
	else shadowMapCount = 6U;
}

vector<mat4> LightObject::GetLightPVMatrices() noexcept
{
	vector<mat4> pvMatrices;

	switch (shadowMapCount)
	{
	default: break;
	case 1:
	{
		float fov = acos(angle[1]);
		float aspect = (shadowWidth == 0.0f || shadowHeight == 0.0f) ? 0.0f : shadowWidth / (float)shadowHeight;
		mat4 projection = glm::perspective(fov, aspect, 0.001f, range * range);

		vec3 position = GetGlobalPosition();
		vec3 up(0, 1, 0);
		mat4 inverse = glm::inverse(GetGlobalRotationMatrix());
		mat4 view = glm::lookAt(position, position + -glm::normalize((vec3)inverse[2]), up);

		pvMatrices.push_back(projection * view);
		break;
	}
	case 6:
	{
		float fov = acos(angle[1]);
		float aspect = (shadowWidth == 0.0f || shadowHeight == 0.0f) ? 0.0f : shadowWidth / (float)shadowHeight;
		mat4 projection = glm::perspective(fov, aspect, 0.001f, range * range);

		vec3 position = GetGlobalPosition();

		pvMatrices.push_back(projection * glm::lookAt(position, position + glm::vec3( 1, 0, 0), glm::vec3(0,-1, 0)));
		pvMatrices.push_back(projection * glm::lookAt(position, position + glm::vec3(-1, 0, 0), glm::vec3(0,-1, 0)));
		pvMatrices.push_back(projection * glm::lookAt(position, position + glm::vec3( 0, 1, 0), glm::vec3(0, 0, 1)));
		pvMatrices.push_back(projection * glm::lookAt(position, position + glm::vec3( 0,-1, 0), glm::vec3(0, 0,-1)));
		pvMatrices.push_back(projection * glm::lookAt(position, position + glm::vec3( 0, 0, 1), glm::vec3(0,-1, 0)));
		pvMatrices.push_back(projection * glm::lookAt(position, position + glm::vec3( 0, 0,-1), glm::vec3(0,-1, 0)));
		break;
	}
	}

	return pvMatrices;
}


void LightObject::DrawDebug()
{
	Colour debugColour = Colour(colour.x, colour.y, colour.z);

	if (angle[1] == 1.0f) // Check for no angle. 1.0 is the cosine of 0 degrees
	{
		debug->lines.AddSphere(GetGlobalPosition(), range, 32, debugColour);
	}
	else
	{
		vec3 direction = -glm::normalize((vec3)GetRotationMatrix()[2]);

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
