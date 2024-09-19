#pragma once

#include "MeshRenderer.h"
#include "HabitatData.h"

class HexTileObject;

class HabitatObject : public MeshRenderer
{
public:
	SerialiseAs(HabitatObject);

	using MeshRenderer::MeshRenderer;

	HexTileObject* hexTiles[3] = { nullptr, nullptr, nullptr };

	// Returns the formed habitat if successful, or nullptr if not
	static HabitatObject* AttemptToFormHabitat(HexTileObject* newHexTile) noexcept;

	static vector<HabitatData> habitats;
	static json GetPrefab(string name) noexcept;
	static void ClearPrefabs() noexcept;

private:
	void FormHabitat(HabitatData habitat, HexTileObject* hex1, HexTileObject* hex2, HexTileObject* hex3) noexcept;
	static void AddPrefab(string name);
	static map<string, json> prefabs;
};
