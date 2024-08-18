#include "LinkedHexGrid.h"

#include "Habitat.h"

#include "RandomGen.h"

#include "Debug.h"

void LinkedHexGrid::SerialiseTo(json& jsonObj) const
{
	GameObject3D::SerialiseTo(jsonObj);

	//...
}
void LinkedHexGrid::DeserialiseFrom(const json& jsonObj, GuidGeneration guidOptions)
{
	GameObject3D::DeserialiseFrom(jsonObj, guidOptions);

	//...
}
void LinkedHexGrid::UpdateFrom(const json& jsonObj, GuidGeneration guidOptions)
{
	GameObject3D::UpdateFrom(jsonObj, guidOptions);

	//...
}

void LinkedHexGrid::Initialise() noexcept
{
	GameObject3D::Initialise();

	ifstream availablePrefabFile("Assets\\Prefabs\\AvailableHex.prefab");
	availablePrefabFile >> HexTile::availablePrefab;
	ifstream grassPrefabFile("Assets\\Prefabs\\EucalyptusHex.prefab");
	grassPrefabFile >> HexTile::eucalyptusPrefab;
	ifstream waterPrefabFile("Assets\\Prefabs\\WaterHex.prefab");
	waterPrefabFile >> HexTile::waterPrefab;
	ifstream flowerPrefabFile("Assets\\Prefabs\\FernHex.prefab");
	flowerPrefabFile >> HexTile::fernPrefab;

	ifstream frogHabitatPrefabFile("Assets\\Prefabs\\FrogHabitat.prefab");
	frogHabitatPrefabFile >> Habitat::frogHabitatPrefab;

	centre = (HexTile*)GameObject::InstantiateFrom(HexTile::eucalyptusPrefab, GuidGeneration::New);
	gameObjectManager->Add(centre);
	centre->SetParent(this);
	lookupTable.insert(HexPair(centre->GetHexPos(), centre));
	centre->type = HexType::Eucalyptus;
}

void LinkedHexGrid::UpdateTile(glm::ivec2 position, HexType hexType) noexcept
{
	HexMap::iterator hexTileIt = lookupTable.find(position);
	if (hexTileIt == lookupTable.end())
	{
		Debug::LogWarning("There is no hex tile at the given position.", locationinfo);
		return;
	}

	UpdateTile(hexTileIt->second, hexType);
}
void LinkedHexGrid::UpdateTile(HexTile* hexTile, HexType hexType) noexcept
{
	if (hexTile->type == HexType::Available)
	{
		for (int i = 0; i < 6; i++)
		{
			if (hexTile->adjacent[i] == nullptr)
			{
				AddTile(hexTile, (HexDir)i);
			}
		}
	}

	vec3 position = hexTile->GetPosition();
	switch (hexType)
	{
	case HexType::Available: hexTile->UpdateFrom(HexTile::availablePrefab, GuidGeneration::Keep); break;
	case HexType::Eucalyptus:		 hexTile->UpdateFrom(HexTile::eucalyptusPrefab, GuidGeneration::Keep); break;
	case HexType::Water:		 hexTile->UpdateFrom(HexTile::waterPrefab, GuidGeneration::Keep); break;
	case HexType::Fern:	 hexTile->UpdateFrom(HexTile::fernPrefab, GuidGeneration::Keep); break;
	default: break;
	}
	hexTile->SetPosition(position);

	hexTile->Rotate(glm::radians(Random::Int32(0, 5) * 60.0f), vec3(0, 1, 0));

	for (int i = 0; i < 6; i++)
	{
		HexTile* neighbour1 = hexTile->adjacent[i];
		HexTile* neighbour2 = hexTile->adjacent[i < 5 ? i + 1 : 0];

		if (neighbour1->type == HexType::Available || neighbour1->habitat != nullptr) continue;
		if (neighbour2->type == HexType::Available || neighbour2->habitat != nullptr) continue;

		if (hexTile->type != neighbour1->type &&
			hexTile->type != neighbour2->type &&
			neighbour1->type != neighbour2->type &&
			!Habitat::frogHabitatHasBeenPlaced)
		{
			Habitat* habitat = (Habitat*)GameObject::InstantiateFrom(Habitat::frogHabitatPrefab, GuidGeneration::New);
			gameObjectManager->Add(habitat);
			habitat->SetParent(this);
			habitats.push_back(habitat);
			habitat->FormHabitat(hexTile, neighbour1, neighbour2);
			Habitat::frogHabitatHasBeenPlaced = true;
			break;
		}
	}
}

void LinkedHexGrid::AddTile(glm::ivec2 originPosition, HexDir direction) noexcept
{
	HexMap::iterator hexTileIt = lookupTable.find(originPosition);
	if (hexTileIt == lookupTable.end())
	{
		Debug::LogWarning("There is no hex tile at the given origin position.", locationinfo);
		return;
	}
	AddTile(hexTileIt->second, direction);
}

void LinkedHexGrid::AddTile(HexTile* origin, HexDir direction) noexcept
{
	HexTile* newTile = (*origin)[direction];
	if (newTile != nullptr)
	{
		Debug::LogWarning("Attempted to add a tile in a place that already has one.", locationinfo);
		return;
	}

	newTile = (HexTile*)GameObject::InstantiateFrom(HexTile::availablePrefab, GuidGeneration::New);
	gameObjectManager->Add(newTile);
	newTile->SetParent(this);
	newTile->SetPosition(HexTile::HexPosToRealPos(origin->GetHexPos() + HexTile::DirVec[(uint)direction]));

	for (uint i = 0; i < 6; i++)
	{
		HexMap::iterator hexTileIt = lookupTable.find(newTile->GetHexPos() + HexTile::DirVec[i]);
		if (hexTileIt != lookupTable.end())
		{
			(*newTile)[(HexDir)i] = hexTileIt->second;
			(*hexTileIt->second)[HexTile::OppositeDir((HexDir)i)] = newTile;
		}
	}

	lookupTable.insert(HexPair(newTile->GetHexPos(), newTile));
}
