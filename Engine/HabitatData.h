#pragma once

#include "JsonIncludes.h"

#include "TileData.h"

#include <string>
using std::string;
#include <vector>
using std::vector;

struct HabitatData
{
	string name = "New Habitat";
	TileID requiredTiles[3] = { TileID(), TileID(), TileID() };
	string prefabFilepath = "None";
	bool hasBeenPlaced = false;
	bool prefabLoaded = false;

	static HabitatData& Get(int index) noexcept;

	static const json& GetPrefab(int index) noexcept;
	static const json& GetPrefab(string name) noexcept;
	static int GetPrefabIndex(string name) noexcept;

	static void ClearAndReset() noexcept;

	static vector<HabitatData> habitatsData;

private:
	static void PopulatePrefab(int index) noexcept;

	json prefab;
};
