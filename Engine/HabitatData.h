#pragma once

#include <string>
using std::string;
#include <vector>
using std::vector;

struct HabitatData
{
	string name = "New Habitat";
	string requiredTiles[3] = { "Undefined", "Undefined", "Undefined" };
	string prefabFilepath = "None";
	bool hasBeenPlaced = false;

	static vector<HabitatData> data;
};
