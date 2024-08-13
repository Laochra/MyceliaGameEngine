#include "Habitat.h"

#include "HexTile.h"

void Habitat::SerialiseTo(json& jsonObj) const
{
	MeshRenderer::SerialiseTo(jsonObj);

	//...
}
void Habitat::DeserialiseFrom(const json& jsonObj, GuidGeneration guidOptions)
{
	MeshRenderer::DeserialiseFrom(jsonObj);

	//...
}
void Habitat::UpdateFrom(const json& jsonObj, GuidGeneration guidOptions)
{
	MeshRenderer::UpdateFrom(jsonObj);

	//...
}

void Habitat::FormHabitat(HexTile* hex1, HexTile* hex2, HexTile* hex3) noexcept
{
	// Ensure the 3 hexes are connected or early out
	uint pairsFound = 0U;
	for (HexTile* neighbour : hex1->adjacent)
	{
		if (neighbour == hex2) { pairsFound++; break; }
	}
	for (HexTile* neighbour : hex2->adjacent)
	{
		if (neighbour == hex3) { pairsFound++; break; }
	}
	for (HexTile* neighbour : hex3->adjacent)
	{
		if (neighbour == hex1) { pairsFound++; break; }
	}
	if (pairsFound != 3U) return;

	// If the habitat is being moved, set existing tiles to be basic land tiles
	for (HexTile* hexTile : hexTiles)
	{
		hexTile->UpdateFrom(HexTile::grassPrefab, GuidGeneration::Keep);
	}

	// Update tiles its made up of
	hexTiles[0] = hex1;
	hexTiles[1] = hex2;
	hexTiles[2] = hex3;

	// Set the position to be the average of the three hexes it makes up
	SetPosition((hex1->GetPosition() + hex2->GetPosition() + hex3->GetPosition()) / 3.0f);
}
