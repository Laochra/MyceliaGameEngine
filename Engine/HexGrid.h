#pragma once

#include "MathIncludes.h"
#include "Debug.h"

constexpr double sqrt3 = 1.7320508075689;
constexpr double invSqrt3 = 1 / 1.7320508075689;

struct HexOffsetCoord
{
	int x, y;

	constexpr HexOffsetCoord(int xInit = 0, int yInit = 0) noexcept;
};
HexOffsetCoord operator+(HexOffsetCoord a, HexOffsetCoord b) noexcept;
HexOffsetCoord operator-(HexOffsetCoord a, HexOffsetCoord b) noexcept;

struct HexCubeCoord
{
	int q, r, s;

	constexpr HexCubeCoord(int qInit = 0, int rInit = 0, int sInit = 0) noexcept :
		q(qInit),
		r(rInit),
		s(sInit) {}
};
HexCubeCoord operator-(HexCubeCoord a, HexCubeCoord b) noexcept;

struct OHexTile
{
	HexOffsetCoord hexCoord;
};

template <int Rows>
struct HexColumn
{
	OHexTile tiles[Rows];
};

enum class HexDirection {
					North,
	NorthWest,				NorthEast,
	SouthWest,				SouthEast,
					South,
};

// Implemented with Offset Coordinates
template <int Columns, int Rows>
class HexGrid
{
	static_assert(Columns & 1 == 1 && Columns > 0 && Rows & 1 == 1 && Rows > 0, "Columns/Rows for HexGrid must be odd and >0");

public:
	static constexpr int columns = Columns;
	static constexpr int rows = Rows;
	static constexpr int minRadius = std::min((Columns - 1)/2, (Rows - 1)/2);

	static const inline HexOffsetCoord centre{Columns / 2, Rows / 2};
	static const inline HexOffsetCoord neighbourOffsets[12]{
		// Even Column Coords
					{ 0, -1},
		{-1, -1},			{+1, -1},
		{-1,  0},			{+1,  0},
					{ 0, +1},
		// Odd Column Coords
					{ 0, -1},
		{-1,  0},			{+1,  0},
		{-1, +1},			{+1, +1},
					{ 0, +1},
	};

	HexColumn<Rows> tiles[Columns];
	OHexTile& operator[](HexOffsetCoord hexCoord) noexcept
	{
		return tiles[hexCoord.x][hexCoord.y];
	}

	static HexOffsetCoord GetNeighbourCoord(HexOffsetCoord hexCoord, HexDirection direction) noexcept
	{
		/// (hexTile.x & 1) returns 0 if even, 1 if odd. More Info: https://en.wikipedia.org/wiki/Two's_complement
		/// We use this to either use the first set of offsets or the second.
		return hexCoord + neighbourOffsets[6 * (hexCoord.x & 1) + (int)direction];
	}

	static HexOffsetCoord CubeToOffset(HexCubeCoord cubeCoords)
	{
		/// (cubeCoords.q & 1) returns 0 if even, 1 if odd. More Info: https://en.wikipedia.org/wiki/Two's_complement
		int x = cubeCoords.q;
		int y = cubeCoords.r + (cubeCoords.q - (cubeCoords.q & 1)) / 2;
		return HexOffsetCoord(x, y) + centre;
	}
	static HexCubeCoord OffsetToCube(HexOffsetCoord offsetCoords)
	{
		/// (offsetCoords.x & 1) returns 0 if even, 1 if odd. More Info: https://en.wikipedia.org/wiki/Two's_complement
		offsetCoords = offsetCoords - centre;
		int q = offsetCoords.x;
		int r = offsetCoords.y - (offsetCoords.x - (offsetCoords.x & 1)) / 2;
		int s = -q-r;
		return HexCubeCoord(q, r, s);
	}

	static vec2 CoordToPos(HexOffsetCoord hexOffsetCoord) noexcept
	{
		return CoordToPos(OffsetToCube(hexOffsetCoord));
	}
	static vec2 CoordToPos(HexCubeCoord hexCubeCoord) noexcept
	{
		float x = 3.0f / 2.0f * hexCubeCoord.q * invSqrt3;
		float y = 1.0f / 2.0f * hexCubeCoord.q + hexCubeCoord.r;
		return vec2(x, y);

		///// (hexTile.x & 1) returns 0 if even, 1 if odd. More Info: https://en.wikipedia.org/wiki/Two's_complement
		///// We use this to shift odd columns by 0.5.
		//return vec2(3.0f / 2.0f * hexCubeCoord.x * (float)invSqrt3, (hexCubeCoord.y + 0.5f * (hexCubeCoord.x & 1)));
	}

	static HexOffsetCoord PosToCoord(vec2 position) noexcept
	{
		float q = 2.0f / 3.0f * position.x * (float)sqrt3;
		float r = position.y - q / 2;
		float s = -q-r;
		return CubeToOffset(RoundedCubeCoords(vec3(q, r, s)));

		///// (hexTile.x & 1) returns 0 if even, 1 if odd. More Info: https://en.wikipedia.org/wiki/Two's_complement
		///// We use this to shift odd columns by 0.5.
		//vec2 remappedPosition(2.0f / 3.0f * position.x * (float)sqrt3, position.y - 0.5f * ((int)std::nearbyintf(position.y * 2) & 1));	
		//return HexOffsetCoord(std::nearbyintf(remappedPosition.x), std::nearbyintf(remappedPosition.y));
	}

private:
	static HexCubeCoord RoundedCubeCoords(vec3 unrounded)
	{
		HexCubeCoord cubeCoords(
			std::nearbyintf(unrounded.x),
			std::nearbyintf(unrounded.y),
			std::nearbyintf(unrounded.z)
		);
		vec3 diff(
			abs(cubeCoords.q - unrounded.x),
			abs(cubeCoords.r - unrounded.y),
			abs(cubeCoords.s - unrounded.z)
		);

		if (diff.x > diff.y && diff.x > diff.z) cubeCoords.q = -cubeCoords.r - cubeCoords.s;
		else if (diff.y > diff.z) cubeCoords.r = -cubeCoords.q - cubeCoords.s;
		else cubeCoords.s = -cubeCoords.q - cubeCoords.r;

		return cubeCoords;
	}
};
