#include "HexGrid.h"

constexpr HexCoord::HexCoord(int xInit, int yInit) noexcept :
	x(xInit),
	y(yInit) {}
HexCoord operator+(HexCoord a, HexCoord b)
{
	return HexCoord(a.x + b.x, a.y + b.y);
}
