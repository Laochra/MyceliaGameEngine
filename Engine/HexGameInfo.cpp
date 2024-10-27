#include "HexGameInfo.h"

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
