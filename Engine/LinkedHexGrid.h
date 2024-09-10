#pragma once

#include "HexTile.h"

#include "MathIncludes.h"

#include <unordered_map>

#include "GameObject3D.h"

typedef unsigned int uint;
typedef unsigned long long ulong;

class Habitat;

struct HexPosHash
{
	ulong operator()(const glm::ivec2& k) const
	{
		ulong x = *(uint*)&k.x;
		ulong z = *(uint*)&k.y;
		return (x << 32) + z;
	}
};

typedef std::unordered_map<glm::ivec2, HexTile*, HexPosHash> HexMap;
typedef std::pair<glm::ivec2, HexTile*> HexPair;

class LinkedHexGrid : public GameObject3D
{
public:
	SerialiseAs(LinkedHexGrid)

	using GameObject3D::GameObject3D;

	HexTile* centre = nullptr;
	HexMap lookupTable;
	vector<Habitat*> habitats;

	virtual void Initialise() noexcept override;

	void UpdateTile(glm::ivec2 position, json tilePrefab) noexcept;
	void UpdateTile(HexTile* hexTile, json tilePrefab) noexcept;

	void AddCentre() noexcept;
	void AddTile(glm::ivec2 originPosition, HexDir direction) noexcept;
	void AddTile(HexTile* origin, HexDir direction) noexcept;

private:
	void EnsurePerimeterIsPlacable() noexcept;
};
