#pragma once

#include "HexType.h"

#include <string>
using std::string;
#include <vector>
using std::vector;
typedef unsigned int uint;

struct TileData
{
	string name = "New Variant";
	string prefabFilepath = "None";
	uint countPlaced = 0U;
	bool unlocked = false;

	TileData() noexcept = default;
	TileData(const char* const nameInit) noexcept;
	
	static vector<TileData>& GetVariantsOfType(HexType hexType) noexcept;

	static vector<vector<TileData>> data;
};
