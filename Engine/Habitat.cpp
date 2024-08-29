#include "Habitat.h"

#include "HexTile.h"

void Habitat::SerialiseTo(json& jsonObj) const
{
	MeshRenderer::SerialiseTo(jsonObj);

	//...
}
void Habitat::DeserialiseFrom(const json& jsonObj, GuidGeneration guidOptions)
{
	MeshRenderer::DeserialiseFrom(jsonObj, guidOptions);

	//...
}
void Habitat::UpdateFrom(const json& jsonObj, GuidGeneration guidOptions)
{
	MeshRenderer::UpdateFrom(jsonObj, guidOptions);

	//...
}

vector<HabitatData> Habitat::habitats;
map<string, json> Habitat::prefabs;


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

	// If the habitat is being moved, set existing tiles to be the default tile
	if (hexTiles[0] != nullptr)
	{
		for (HexTile* hexTile : hexTiles)
		{
			hexTile->UpdateFrom(HexTile::GetDefaultTilePrefab(), GuidGeneration::Keep);
			hexTile->SetState(Active);
			hexTile->habitat = nullptr;
		}
	}

	// Update tiles its made up of
	hexTiles[0] = hex1;
	hexTiles[1] = hex2;
	hexTiles[2] = hex3;
	for (HexTile* hexTile : hexTiles)
	{
		hexTile->SetState(Inactive);
		hexTile->habitat = this;
	}

	// Set the position to be the average of the three hexes it makes up
	SetPosition((hex1->GetPosition() + hex2->GetPosition() + hex3->GetPosition()) / 3.0f);

	// Rotate based on the layout of the 3 hexes
	uint indicesByDepth[3] { 0U, 0U, 0U };

	if (hexTiles[1]->GetPosition().z < hexTiles[0]->GetPosition().z)
	{
		indicesByDepth[1] = 1U;
	}
	else
	{
		indicesByDepth[0] = 1U;
	}
	if (hexTiles[2]->GetPosition().z < hexTiles[indicesByDepth[1]]->GetPosition().z)
	{
		indicesByDepth[2] = 2U;
	}
	else
	{
		indicesByDepth[2] = indicesByDepth[1];

		if (hexTiles[2]->GetPosition().z < hexTiles[indicesByDepth[0]]->GetPosition().z)
		{
			indicesByDepth[1] = 2U;
		}
		else
		{
			indicesByDepth[1] = indicesByDepth[0];
			indicesByDepth[0] = 2U;
		}
	}

	// If furthest depth tile is to the left, rotate the habitat
	if (hexTiles[indicesByDepth[2]]->GetPosition().x < hexTiles[indicesByDepth[1]]->GetPosition().x)
	{
		Rotate(glm::radians(-60.0f), vec3(0, 1, 0));
	}
}

json Habitat::GetPrefab(string name) noexcept
{
	if (!prefabs.contains(name))
	{
		AddPrefab(name);
	}

	return prefabs[name];
}

void Habitat::AddPrefab(string name)
{
	json prefab;
	string prefabFilepath = "None";


	for (HabitatData habitat : Habitat::habitats)
	{
		if (name == habitat.name)
		{
			prefabFilepath = habitat.prefabFilepath;
			break;
		}
	}

	bool successfulSoFar = true;

	if (prefabFilepath != "None")
	{
		ifstream prefabFile(prefabFilepath);
		if (!prefabFile.good())
		{
			Debug::LogWarning(LogID::WRN101, "Habitat: ", name, ". It will instead use the default tile file.", locationinfo);
			successfulSoFar = false;
		}
		else
		{
			try { prefabFile >> prefab; }
			catch (parse_error)
			{
				Debug::LogWarning(LogID::WRN102, "Habitat: ", name, ". It will instead use the default tile file.", locationinfo);
				successfulSoFar = false;
			}
		}
	}

	if (successfulSoFar)
	{
		if (prefab["Name"] != name)
		{
			prefab["Name"] = name;
			ofstream updatedFile(prefabFilepath);
			updatedFile << prefab;
		}
	}
	else
	{
		prefab = prefabs["Default"];
		prefab["Name"] = name;
		prefab.erase("HexType");
		prefab.erase("HexVariant");
		prefab["TypeID"] = Habitat::classID;
	}

	prefabs.insert(std::pair(name, prefab));
}
