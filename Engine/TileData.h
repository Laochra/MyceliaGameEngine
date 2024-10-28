#pragma once

#include "HexType.h"

#include "JsonIncludes.h"

#include <string>
using std::string;
#include <vector>
using std::vector;

typedef unsigned int uint;

class UISprite;

class TileData
{
public:
	string name = "New Variant";
	string prefabFilepath = "None";
	string spriteFilepath = "None";
	uint countPlaced = 0U;
	bool unlocked = false;
	bool prefabLoaded = false;

	TileData() noexcept = default;
	TileData(const char* const nameInit) noexcept;
	
	static std::array<vector<TileData>, 4> tilesData;
	static std::array<string, 3> defaultTilePaths;
	static std::array<json, 3> defaultTilePrefabs;

	static vector<TileData>& Get(HexType hexType) noexcept;
	static TileData& Get(HexType hexType, char variant) noexcept;

	static char GetVariantIndex(HexType hexType, string name) noexcept;

	static const json* GetDefaultPrefab() noexcept;
	static const json* GetPerimeterPrefab() noexcept;
	static const json* GetMotherTreePrefab() noexcept;
	static const json* GetPrefab(string name) noexcept;

	static UISprite* selectedTile;

	static void ClearAndReset() noexcept;

private:
	static void PopulatePrefab(HexType hexType, char variant) noexcept;
	static void PopulateDefaultPrefab(int index) noexcept;

	json prefab;
};
