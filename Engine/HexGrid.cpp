#include "HexGrid.h"

constexpr HexOffsetCoord::HexOffsetCoord(int xInit, int yInit) noexcept :
	x(xInit),
	y(yInit) {}
HexOffsetCoord operator+(HexOffsetCoord a, HexOffsetCoord b) noexcept
{
	return HexOffsetCoord(a.x + b.x, a.y + b.y);
}

HexOffsetCoord operator-(HexOffsetCoord a, HexOffsetCoord b) noexcept
{
	return HexOffsetCoord(a.x - b.x, a.y - b.y);
}

HexCubeCoord operator-(HexCubeCoord a, HexCubeCoord b) noexcept
{
	return HexCubeCoord(a.q - b.q, a.r - b.r, a.s - b.s);
}
