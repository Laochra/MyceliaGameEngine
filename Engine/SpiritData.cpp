#include "SpiritData.h"

#include "Debug.h"

vector<SpiritData> SpiritData::spiritsData;

SpiritData& SpiritData::Get(int index) noexcept
{
	return spiritsData[index];
}

const json* SpiritData::GetPrefab(int index) noexcept
{
	SpiritData& spirit = spiritsData[index];

	if (!spirit.prefabLoaded) PopulatePrefab(index);
	return &spirit.prefab;
}

const json* SpiritData::GetPrefab(string name) noexcept
{
	int index = GetPrefabIndex(name);

	if (index < 0) return nullptr;

	SpiritData& spirit = spiritsData[index];

	if (!spirit.prefabLoaded) PopulatePrefab(index);
	return &spirit.prefab;
}

int SpiritData::GetPrefabIndex(string name) noexcept
{
	for (int h = 0; h < (int)spiritsData.size(); h++)
	{
		SpiritData& spirit = spiritsData[h];
		if (name == spirit.name)
		{
			return h;
		}
	}

	return -1;
}

void SpiritData::ClearAndReset() noexcept
{
	for (int h = 0; h < (int)spiritsData.size(); h++)
	{
		SpiritData& spirit = spiritsData[h];
		spirit.prefabLoaded = false;
		spirit.prefab.clear();
	}
}

void SpiritData::PopulatePrefab(int index) noexcept
{
	SpiritData& spiritData = Get(index);

	if (spiritData.prefabFilepath == "None")
	{
		Debug::LogWarning(spiritData.name, " doesn't have a prefab specified.", locationinfo);
		return;
	}

	ifstream prefabFile(spiritData.prefabFilepath);

	if (!prefabFile.good())
	{
		Debug::LogWarning(LogID::WRN101, spiritData.name, locationinfo);
		return;
	}

	try { prefabFile >> spiritData.prefab; }
	catch (parse_error)
	{
		Debug::LogWarning(LogID::WRN102, spiritData.name, locationinfo);
		return;
	}

	if (spiritData.prefab["Name"] != spiritData.name)
	{
		spiritData.prefab["Name"] = spiritData.name;
		ofstream updatedFile(spiritData.prefabFilepath);
		updatedFile << std::setw(2) << spiritData.prefab;
	}
}
