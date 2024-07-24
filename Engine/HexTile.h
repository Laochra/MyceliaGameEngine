#pragma once

#include "MathIncludes.h"
#include <unordered_map>
#include <vector>

enum class HexDir : unsigned char
{
	NorthWest,
	North,
	NorthEast,
	SouthEast,
	South,
	SouthWest
};

class HexTile
{
public:
	vec3 position;
	HexTile* adjacent[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, };

	HexTile() noexcept = default;
	HexTile(vec3 positionInit) noexcept;

	HexTile*& operator[](HexDir direction) noexcept;

	const static std::vector<vec3> DirVec;
	static HexDir OppositeDir(HexDir dir);
};
