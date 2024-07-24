#include "HexTile.h"

const std::vector<vec3> HexTile::DirVec =
{
	vec3(-0.866f,	0, -0.5f),
	vec3( 0,			0, -1   ),
	vec3( 0.866f,	0, -0.5f),
	vec3( 0.866f,	0,  0.5f),
	vec3( 0,			0,  1   ),
	vec3(-0.866f,	0,  0.5f)
};

HexTile::HexTile(vec3 positionInit) noexcept : position(positionInit) { }

HexTile*& HexTile::operator[](HexDir direction) noexcept
{
	return adjacent[(unsigned int)direction];
}

HexDir HexTile::OppositeDir(HexDir dir)
{
	switch (dir)
	{
	case HexDir::NorthWest: return HexDir::SouthEast;
	case HexDir::North:		return HexDir::South;
	case HexDir::NorthEast: return HexDir::SouthWest;
	case HexDir::SouthEast: return HexDir::NorthWest;
	case HexDir::South:		return HexDir::North;
	case HexDir::SouthWest:	return HexDir::NorthEast;
	default:						return (HexDir)0;
	}
}
