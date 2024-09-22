#include "HabitatData.h"

#include "TileData.h"

#include "Debug.h"

vector<HabitatData> HabitatData::habitatsData;

HabitatData& HabitatData::Get(int index) noexcept
{
	return habitatsData[index];
}

const json& HabitatData::GetPrefab(int index) noexcept
{
	HabitatData& habitat = habitatsData[index];

	if (!habitat.prefabLoaded) PopulatePrefab(index);
	return habitat.prefab;
}
const json& HabitatData::GetPrefab(string name) noexcept
{
	int index = GetPrefabIndex(name);

	if (index < 0) return TileData::GetDefaultPrefab();

	HabitatData& habitat = habitatsData[index];

	if (!habitat.prefabLoaded) PopulatePrefab(index);
	return habitat.prefab;
}
int HabitatData::GetPrefabIndex(string name) noexcept
{
	for (int h = 0; h < (int)habitatsData.size(); h++)
	{
		HabitatData& habitat = habitatsData[h];
		if (name == habitat.name)
		{
			return h;
		}
	}

	return -1;
}

void HabitatData::ClearAndReset() noexcept
{
	for (int h = 0; h < (int)habitatsData.size(); h++)
	{
		HabitatData& habitat = habitatsData[h];
		habitat.hasBeenPlaced = false;
		habitat.prefabLoaded = false;
		habitat.prefab.clear();
	}
}

void HabitatData::PopulatePrefab(int index) noexcept
{
	HabitatData& habitatData = Get(index);

	if (habitatData.prefabFilepath == "None")
	{
		Debug::LogWarning(habitatData.name, " doesn't have a prefab specified. It will instead use the default tile file.", locationinfo);
		habitatData.prefab = TileData::GetDefaultPrefab();
		return;
	}

	ifstream prefabFile(habitatData.prefabFilepath);

	if (!prefabFile.good())
	{
		Debug::LogWarning(LogID::WRN101, habitatData.name, ". It will instead use the default tile file.", locationinfo);
		habitatData.prefab = TileData::GetDefaultPrefab();
		return;
	}

	try { prefabFile >> habitatData.prefab; }
	catch (parse_error)
	{
		Debug::LogWarning(LogID::WRN102, habitatData.name, ". It will instead use the default tile file.", locationinfo);
	}

	if (habitatData.prefab["Name"] != habitatData.name)
	{
		habitatData.prefab["Name"] = habitatData.name;
		ofstream updatedFile(habitatData.prefabFilepath);
		updatedFile << std::setw(2) << habitatData.prefab;
	}
}
