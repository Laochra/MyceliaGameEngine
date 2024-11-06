#include "HexGameInfo.h"

#include "GameObject3D.h"
#include "LightObject.h"

#include "Debug.h"

vec4 HexGameInfo::highlightColour = vec4(1.0f, 1.0f, 1.0f, 1.0f);

std::string HexGameInfo::backgroundFilepath = "None";
json HexGameInfo::backgroundPrefab;
GameObject3D* HexGameInfo::background = nullptr;

void HexGameInfo::LoadBackgroundPrefab(const string& filepath)
{
	if (filepath != "None")
	{
		ifstream prefabFile(filepath);

		if (!prefabFile.good())
		{
			Debug::LogWarning(LogID::WRN101, "Background Prefab.", locationinfo);
			return;
		}

		try { prefabFile >> HexGameInfo::backgroundPrefab; }
		catch (parse_error)
		{
			Debug::LogWarning(LogID::WRN102, "Background Prefab", locationinfo);
			return;
		}

		HexGameInfo::backgroundFilepath = filepath;
	}
}

std::string HexGameInfo::lightingFilepath = "None";
json HexGameInfo::lightingPrefab;
LightObject* HexGameInfo::lighting = nullptr;
void HexGameInfo::LoadLightingPrefab(const string& filepath)
{
	if (filepath != "None")
	{
		ifstream prefabFile(filepath);

		if (!prefabFile.good())
		{
			Debug::LogWarning(LogID::WRN101, "Lighting Prefab.", locationinfo);
			return;
		}

		try { prefabFile >> HexGameInfo::lightingPrefab; }
		catch (parse_error)
		{
			Debug::LogWarning(LogID::WRN102, "Lighting Prefab", locationinfo);
			return;
		}

		HexGameInfo::lightingFilepath = filepath;
	}
}
