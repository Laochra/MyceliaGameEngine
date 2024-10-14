#include "Habitat.h"

#include "TileData.h"
#include "HabitatData.h"

#include "HexScrapbook.h"
#include "HexProgression.h"

#include "GameObject3D.h"
#include "LinkedHexGrid.h"

Habitat Habitat::AttemptToFormHabitat(LinkedHexGrid* hexGrid, HexOffsetCoord newHexCoords) noexcept
{
	HexTile& newHexTile = hexGrid->Get(newHexCoords);

	Habitat newHabitat;

	HexDirection directions[6] {
		HexDirection::North,
		HexDirection::NorthEast,
		HexDirection::SouthEast,
		HexDirection::South,
		HexDirection::SouthWest,
		HexDirection::NorthWest
	};
	for (int i = 0; i < 6; i++)
	{
		HexOffsetCoord neighbour1Coords = HexOffsetCoord::GetCoords(newHexCoords, directions[i]);
		HexOffsetCoord neighbour2Coords = HexOffsetCoord::GetCoords(newHexCoords, directions[i < 5 ? i + 1 : 0]);

		HexTile& neighbour1 = hexGrid->Get(neighbour1Coords);
		HexTile& neighbour2 = hexGrid->Get(neighbour2Coords);

		if ((char)neighbour1.type < 0 || neighbour1.habitat >= 0) continue;
		if ((char)neighbour2.type < 0 || neighbour2.habitat >= 0) continue;

		bool habitatDataFound = false;
		for (HabitatData& habitatData : HabitatData::habitatsData)
		{
			if (habitatData.hasBeenPlaced) continue;

			vector<string> variantsToCheckFor {
				habitatData.requiredTiles[0],
				habitatData.requiredTiles[1],
				habitatData.requiredTiles[2]
			};

			for (vector<string>::iterator it = variantsToCheckFor.begin(); it != variantsToCheckFor.end();)
			{
				if (TileData::Get(newHexTile.type, newHexTile.variant).name == *it)
				{
					it = variantsToCheckFor.erase(it);
					break;
				}
				else
				{
					it++;
				}
			}
			for (vector<string>::iterator it = variantsToCheckFor.begin(); it != variantsToCheckFor.end();)
			{
				if (TileData::Get(neighbour1.type, neighbour1.variant).name == *it)
				{
					it = variantsToCheckFor.erase(it);
					break;
				}
				else
				{
					it++;
				}
			}
			for (vector<string>::iterator it = variantsToCheckFor.begin(); it != variantsToCheckFor.end();)
			{
				if (TileData::Get(neighbour2.type, neighbour2.variant).name == *it)
				{
					it = variantsToCheckFor.erase(it);
					break;
				}
				else
				{
					it++;
				}
			}

			if (variantsToCheckFor.size() == 0)
			{
				int habitatIndex = HabitatData::GetPrefabIndex(habitatData.name);

				newHabitat.FormHabitat(hexGrid, habitatIndex, newHexCoords, neighbour1Coords, neighbour2Coords);
				habitatData.hasBeenPlaced = true;
				habitatDataFound = true;
				break;
			}
		}
		if (habitatDataFound) break;
	}

	return newHabitat;
}

void Habitat::FormHabitat(LinkedHexGrid* hexGrid, int habitatIndex, HexOffsetCoord hexCoord1, HexOffsetCoord hexCoord2, HexOffsetCoord hexCoord3) noexcept
{
	habitatID = habitatIndex;

	HabitatData& habitatData = HabitatData::Get(habitatIndex);
	HexScrapbook::RevealHabitat(habitatIndex);
	HexProgression::currentRadius += 2;

	HexTile& hex1 = hexGrid->Get(hexCoord1);
	HexTile& hex2 = hexGrid->Get(hexCoord2);
	HexTile& hex3 = hexGrid->Get(hexCoord3);

	// If the habitat is being moved, set existing tiles to be the default tile
	if (object != nullptr)
	{
		for (HexOffsetCoord hexCoord : tileCoords)
		{
			HexTile& hexTile = hexGrid->Get(hexCoord);
			TileData::Get(hexTile.type, hexTile.variant).countPlaced--;
			json defaultPrefab = *TileData::GetDefaultPrefab();
			hexTile.object->UpdateFrom(defaultPrefab, GuidGeneration::Keep);
			hexTile.type = defaultPrefab["HexType"];
			hexTile.variant = TileData::GetVariantIndex(hexTile.type, defaultPrefab["HexVariant"]);

			hexTile.object->SetState(GameObject::Active);
			hexTile.habitat = habitatIndex;
		}
	}
	else
	{
		// Instantiate the habitat object
		object = (GameObject3D*)GameObject::InstantiateFrom(HabitatData::GetPrefab(habitatIndex), GuidGeneration::New);
	}

	// Update tiles its made up of
	tileCoords[0] = hexCoord1;
	tileCoords[1] = hexCoord2;
	tileCoords[2] = hexCoord3;
	for (HexOffsetCoord hexCoord : tileCoords)
	{
		HexTile& hexTile = hexGrid->Get(hexCoord);
		hexTile.object->SetState(GameObject::Inactive);
		hexTile.habitat = habitatIndex;
	}

	// Set the position to be the average of the three hexes it makes up
	object->SetPosition((hex1.object->GetPosition() + hex2.object->GetPosition() + hex3.object->GetPosition()) / 3.0f);

	// Rotate based on the layout of the 3 hexes
	uint indicesByDepth[3]{ 0U, 0U, 0U };

	if (hex2.object->GetPosition().z < hex1.object->GetPosition().z)
	{
		indicesByDepth[1] = 1U;
	}
	else
	{
		indicesByDepth[0] = 1U;
	}
	if (hex3.object->GetPosition().z < hexGrid->Get(tileCoords[indicesByDepth[1]]).object->GetPosition().z)
	{
		indicesByDepth[2] = 2U;
	}
	else
	{
		indicesByDepth[2] = indicesByDepth[1];

		if (hex3.object->GetPosition().z < hexGrid->Get(tileCoords[indicesByDepth[0]]).object->GetPosition().z)
		{
			indicesByDepth[1] = 2U;
		}
		else
		{
			indicesByDepth[1] = indicesByDepth[0];
			indicesByDepth[0] = 2U;
		}
	}

	// If furthest depth tile is to the left, rotate the habitat clockwise, otherwise rotate anti-clockwise
	if (hexGrid->Get(tileCoords[indicesByDepth[2]]).object->GetPosition().x < hexGrid->Get(tileCoords[indicesByDepth[1]]).object->GetPosition().x)
	{
		object->Rotate(glm::radians(-30.0f), vec3(0, 1, 0));
	}
	else
	{
		object->Rotate(glm::radians(30.0f), vec3(0, 1, 0));
	}
}
