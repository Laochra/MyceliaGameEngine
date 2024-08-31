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


Habitat* Habitat::AttemptToFormHabitat(HexTile* newHexTile) noexcept
{
	Habitat* newHabitat = nullptr;

	for (int i = 0; i < 6; i++)
	{
		HexTile* neighbour1 = newHexTile->adjacent[i];
		HexTile* neighbour2 = newHexTile->adjacent[i < 5 ? i + 1 : 0];
	
		if (neighbour1->type == HexType::Empty || neighbour1->habitat != nullptr) continue;
		if (neighbour2->type == HexType::Empty || neighbour2->habitat != nullptr) continue;
		
		map<string, int> variantCounts;
		variantCounts[newHexTile->variant] += 1;
		variantCounts[neighbour1->variant] += 1;
		variantCounts[neighbour2->variant] += 1;
		
		bool habitatDataFound = false;
		for (HabitatData& habitatData : Habitat::habitats)
		{
			if (habitatData.hasBeenPlaced) continue;

			map<string, int> variantsToCheckFor;
			variantsToCheckFor[habitatData.requiredTiles[0]] += 1;
			variantsToCheckFor[habitatData.requiredTiles[1]] += 1;
			variantsToCheckFor[habitatData.requiredTiles[2]] += 1;

			bool matchFound = true;
			for (std::pair<string, int> variantCount : variantCounts)
			{
				if (variantCount.second != variantsToCheckFor.count(variantCount.first))
				{
					matchFound = false;
					break;
				}
			}

			if (matchFound)
			{
				newHabitat = GameObject::Instantiate<Habitat>();
				newHabitat->UpdateFrom(Habitat::GetPrefab(habitatData.name));
				newHabitat->FormHabitat(habitatData, newHexTile, neighbour1, neighbour2);
				habitatData.hasBeenPlaced = true;
				habitatDataFound == true;
				break;
			}
		}
		if (habitatDataFound) break;
	}

	return newHabitat;
}

void Habitat::FormHabitat(HabitatData habitatData, HexTile* hex1, HexTile* hex2, HexTile* hex3) noexcept
{
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

	// If furthest depth tile is to the left, rotate the habitat clockwise, otherwise rotate anti-clockwise
	if (hexTiles[indicesByDepth[2]]->GetPosition().x < hexTiles[indicesByDepth[1]]->GetPosition().x)
	{
		Rotate(glm::radians(-30.0f), vec3(0, 1, 0));
	}
	else
	{
		Rotate(glm::radians(30.0f), vec3(0, 1, 0));
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
			updatedFile << std::setw(2) << prefab;
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
