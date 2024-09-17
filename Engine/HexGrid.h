#pragma once

#include "MathIncludes.h"

#include "Debug.h"

constexpr double sqrt3 = 1.7320508075689;
constexpr double invSqrt3 = 1 / 1.7320508075689;

struct HexCoord
{
	int x, y;

	constexpr HexCoord(int xInit = 0, int yInit = 0) noexcept;
};
HexCoord operator+(HexCoord a, HexCoord b);

struct OHexTile
{
	HexCoord hexCoord;
};

template <int Rows>
struct HexColumn
{
	OHexTile tiles[Rows];
};

enum class HexDirection { NorthWest, North, NorthEast, SouthEast, South, SouthWest };

// Implemented with Offset Coordinates
template <int Columns, int Rows>
class HexGrid
{
public:
	static const inline HexCoord centre{Columns / 2, Rows / 2};
	static const inline HexCoord neighbourOffsets[12]{
		// Even Column Coords
		{+1,  0}, {+1, -1}, {0, -1},
		{-1, -1}, {-1,  0}, {0, +1},
		// Odd Column Coords
		{+1, +1}, {+1,  0}, {0, -1},
		{-1,  0}, {-1, +1}, {0, +1},
	};

	HexColumn<Rows> tiles[Columns];
	OHexTile& operator[](HexCoord hexCoord) noexcept
	{
		return tiles[hexCoord.x][hexCoord.y];
	}

	HexCoord GetNeighbourCoord(HexCoord hexCoord, HexDirection direction) const noexcept
	{
		/// (hexTile.x & 1) returns 0 if even, 1 if odd. More Info: https://en.wikipedia.org/wiki/Two's_complement
		/// We use this to either use the first set of offsets or the second.
		return hexCoord + HexGrid<Columns, Rows>::neighbourOffsets[6 * (hexCoord.x & 1) + (int)direction];
	}

	static vec2 CoordToPos(HexCoord hexCoord) noexcept
	{
		/// (hexTile.x & 1) returns 0 if even, 1 if odd. More Info: https://en.wikipedia.org/wiki/Two's_complement
		/// We use this to shift odd columns by 0.5.
		return vec2(3.0f / 2.0f * hexCoord.x * (float)invSqrt3, (hexCoord.y + 0.5f * (hexCoord.x & 1)));
	}
	// TODO: This needs to be tested if it will correctly round to the nearest hex. Suspecting it won't, but its a place-holder for testing
	static HexCoord PosToCoord(vec2 position) noexcept
	{
		/// (hexTile.x & 1) returns 0 if even, 1 if odd. More Info: https://en.wikipedia.org/wiki/Two's_complement
		/// We use this to shift odd columns by 0.5.
		vec2 remappedPosition(2.0f / 3.0f * position.x * (float)sqrt3, position.y - 0.5f * ((int)std::nearbyintf(position.y * 2) & 1));
		return HexCoord(std::nearbyintf(remappedPosition.x), std::nearbyintf(remappedPosition.y));
	}
};
