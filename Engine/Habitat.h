#pragma once

#include "MeshRenderer.h"

class HexTile;

struct HabitatData
{
	string name = "New Habitat";
	string requiredTiles[3] = { "Undefined", "Undefined", "Undefined" };
	string prefabFilepath = "None";
	bool hasBeenPlaced = false;
};

class Habitat : public MeshRenderer
{
public:
	SerialiseAs(Habitat);

	using MeshRenderer::MeshRenderer;

	HexTile* hexTiles[3] = { nullptr, nullptr, nullptr };

	void FormHabitat(HexTile* hex1, HexTile* hex2, HexTile* hex3) noexcept;

	static vector<HabitatData> habitats;
	static json GetPrefab(string name) noexcept;

private:
	static void AddPrefab(string name);
	static map<string, json> prefabs;
};
