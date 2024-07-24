#pragma once

#include "HexTile.h"

#include "MathIncludes.h"

#include <unordered_map>

typedef unsigned int uint;
typedef unsigned long long ulong;

struct HexPosHash
{
	ulong operator()(const vec3& k) const
	{
		ulong x = *(uint*)&k.x;
		ulong z = *(uint*)&k.z;
		return (x << 32) + z;
	}
};

typedef std::unordered_map<vec3, HexTile*, HexPosHash> HexMap;
typedef std::pair<vec3, HexTile*> HexPair;

class LinkedHexGrid
{
public:
	HexTile* centre;
	HexMap lookupTable;

	LinkedHexGrid() noexcept;
	~LinkedHexGrid() noexcept;

	void AddTile(vec3 originPosition, HexDir direction) noexcept;
	void AddTile(HexTile* origin, HexDir direction) noexcept;
};
