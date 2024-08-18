#pragma once

#include "MeshRenderer.h"

class HexTile;

class Habitat : public MeshRenderer
{
public:
	SerialiseAs(Habitat);

	using MeshRenderer::MeshRenderer;

	HexTile* hexTiles[3] = { nullptr, nullptr, nullptr };

	void FormHabitat(HexTile* hex1, HexTile* hex2, HexTile* hex3) noexcept;

	static json frogHabitatPrefab;
	static bool frogHabitatHasBeenPlaced;
};
