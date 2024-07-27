#pragma once

#include "HexTile.h"

#include "MathIncludes.h"

#include <unordered_map>

#include "GameObject3D.h"

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

class LinkedHexGrid : public GameObject3D
{
public:
	SerialiseAs(LinkedHexGrid)

	using GameObject3D::GameObject3D;

	HexTile* centre = nullptr;
	HexMap lookupTable;

	virtual void Initialise() noexcept override;

	void AddTile(vec3 originPosition, HexDir direction) noexcept;
	void AddTile(HexTile* origin, HexDir direction) noexcept;
};
