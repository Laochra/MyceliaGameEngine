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
}

void LinkedHexGrid::UpdateTile(glm::ivec2 position, json tilePrefab) noexcept
{
	HexMap::iterator hexTileIt = lookupTable.find(position);
	if (hexTileIt == lookupTable.end())
	{
		Debug::LogWarning("There is no hex tile at the given position.", locationinfo);
		return;
	}

	UpdateTile(hexTileIt->second, tilePrefab);
}
static void ModifyTileCount(HexType type, string variant, uint density, int modification)
{
	vector<TileData>* tileDataList;
	switch (type)
	{
	case HexType::Tree:		tileDataList = &HexTile::trees;		break;
	case HexType::Flower:	tileDataList = &HexTile::flowers;	break;
	case HexType::Water:		tileDataList = &HexTile::waters;		break;
	case HexType::Land:		tileDataList = &HexTile::lands;		break;
	default:						tileDataList = nullptr;					break; // This should never happen
	}
	if (tileDataList != nullptr)
	{
		for (TileData& tileData : *tileDataList)
		{
			if (tileData.name == variant)
			{
				tileData.countPlaced[0] += modification;
			}
		}
	}
}
void LinkedHexGrid::UpdateTile(HexTile* hexTile, json tilePrefab) noexcept
{
	if (hexTile->type == (HexType)(int)tilePrefab["HexType"] &&
		 hexTile->variant == (string)tilePrefab["HexVariant"])
	{
		hexTile->Rotate(glm::radians(-60.0f), vec3(0, 1, 0));
		return;
	}

	if (hexTile->type == HexType::Empty)
	{
		for (int i = 0; i < 6; i++)
		{
			if (hexTile->adjacent[i] == nullptr)
			{
				AddTile(hexTile, (HexDir)i);
			}
		}
	}
	else
	{
		ModifyTileCount(hexTile->type, hexTile->variant, 0, -1);
	}

	vec3 position = hexTile->GetPosition();
	hexTile->UpdateFrom(tilePrefab, GuidGeneration::Keep);
	hexTile->SetPosition(position);

	hexTile->Rotate(glm::radians(Random::Int32(0, 5) * 60.0f), vec3(0, 1, 0));

	hexTile->SetName(StringBuilder(hexTile->variant).CStr());

	ModifyTileCount(hexTile->type, hexTile->variant, 0, +1);

	Habitat* habitat = Habitat::AttemptToFormHabitat(hexTile);
	if (habitat != nullptr)
	{
		habitat->SetParent(this);
		habitats.push_back(habitat);
	}
}

void LinkedHexGrid::AddCentre() noexcept
{
	centre = (HexTile*)GameObject::InstantiateFrom(HexTile::GetDefaultTilePrefab(), GuidGeneration::New);
	gameObjectManager->Add(centre);
	centre->SetParent(this);
	lookupTable.insert(HexPair(centre->GetHexPos(), centre));

	centre->SetName(StringBuilder(centre->variant).CStr());

	AddTile(vec3(0, 0, 0), HexDir::NorthWest);
	AddTile(vec3(0, 0, 0), HexDir::North);
	AddTile(vec3(0, 0, 0), HexDir::NorthEast);
	AddTile(vec3(0, 0, 0), HexDir::SouthEast);
	AddTile(vec3(0, 0, 0), HexDir::South);
	AddTile(vec3(0, 0, 0), HexDir::SouthWest);
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

	newTile = (HexTile*)GameObject::InstantiateFrom(HexTile::GetEmptyTilePrefab(), GuidGeneration::New);
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
