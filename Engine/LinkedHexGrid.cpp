#include "LinkedHexGrid.h"

#include "Debug.h"

LinkedHexGrid::LinkedHexGrid() noexcept
{
	centre = new HexTile(vec3(0, 0, 0));
	lookupTable.insert(HexPair(centre->position, centre));
}

LinkedHexGrid::~LinkedHexGrid() noexcept
{
	for (HexPair hexPair : lookupTable)
	{
		delete hexPair.second;
	}
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

	newTile = new HexTile(origin->position + HexTile::DirVec[(uint)direction]);

	for (uint i = 0; i < 6; i++)
	{
		HexMap::iterator hexTileIt = lookupTable.find(newTile->position + HexTile::DirVec[i]);
		if (hexTileIt != lookupTable.end())
		{
			(*newTile)[(HexDir)i] = hexTileIt->second;
			(*hexTileIt->second)[HexTile::OppositeDir((HexDir)i)] = newTile;
		}
	}

	lookupTable.insert(HexPair(newTile->position, newTile));
}
