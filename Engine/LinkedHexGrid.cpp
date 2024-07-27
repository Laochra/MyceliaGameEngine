#include "LinkedHexGrid.h"

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

void LinkedHexGrid::Initialise() noexcept
{
	GameObject3D::Initialise();

	ifstream availablePrefabFile("Assets\\Prefabs\\AvailableHex.prefab");
	availablePrefabFile >> HexTile::availablePrefab;
	ifstream grassPrefabFile("Assets\\Prefabs\\GrassHex.prefab");
	grassPrefabFile >> HexTile::grassPrefab;
	ifstream waterPrefabFile("Assets\\Prefabs\\WaterHex.prefab");
	waterPrefabFile >> HexTile::waterPrefab;

	centre = (HexTile*)GameObject::InstantiateFrom(HexTile::grassPrefab, GuidGeneration::New);
	gameObjectManager->Add(centre);
	centre->SetParent(this);
	lookupTable.insert(HexPair(centre->GetPosition(), centre));
	centre->type = HexType::Grass;
}

void LinkedHexGrid::AddTile(vec3 originPosition, HexDir direction) noexcept
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
	newTile->SetPosition(origin->GetPosition() + HexTile::DirVec[(uint)direction]);

	for (uint i = 0; i < 6; i++)
	{
		HexMap::iterator hexTileIt = lookupTable.find(newTile->GetPosition() + HexTile::DirVec[i]);
		if (hexTileIt != lookupTable.end())
		{
			(*newTile)[(HexDir)i] = hexTileIt->second;
			(*hexTileIt->second)[HexTile::OppositeDir((HexDir)i)] = newTile;
		}
	}

	lookupTable.insert(HexPair(newTile->GetPosition(), newTile));
}
