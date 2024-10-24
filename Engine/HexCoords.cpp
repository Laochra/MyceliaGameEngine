#include "HexCoords.h"

#include "MathIncludes.h"

const HexOffsetCoord HexOffsetCoord::neighbours[12] {
		// Even Column Coords
				{  0, -1 },
	{ -1, -1 },			{ +1, -1 },
	{ -1,  0 },			{ +1,  0 },
				{  0, +1 },
		// Odd Column Coords
				{  0, -1 },
	{ -1,  0 },			{ +1,  0 },
	{ -1, +1 },			{ +1, +1 },
				{  0, +1 },
};

HexOffsetCoord::HexOffsetCoord(short xInit, short yInit) noexcept :
	x(xInit),
	y(yInit) {}

short HexOffsetCoord::GetMagnitude(HexOffsetCoord centre) const noexcept
{
	return HexOffsetToCube(*this, centre).HexCubeCoord::GetMagnitude();
}

HexOffsetCoord HexOffsetCoord::GetCoords(HexOffsetCoord hexCoord, HexDirection direction) noexcept
{
	/// (hexCoord.x & 1) returns 0 if even, 1 if odd. More Info: https://en.wikipedia.org/wiki/Two's_complement
	/// We use this to either use the first set of offsets or the second.
	return hexCoord + HexOffsetCoord::neighbours[6 * (hexCoord.x & 1) + (int)direction];
}

vec2 HexOffsetCoord::ToPos(HexOffsetCoord hexOffsetCoord, HexOffsetCoord centre) noexcept
{
	return HexCubeCoord::ToPos(HexOffsetToCube(hexOffsetCoord, centre));
}

HexOffsetCoord HexOffsetCoord::GetFromPos(vec2 position, HexOffsetCoord centre) noexcept
{
	return HexCubeToOffset(HexCubeCoord::GetFromPos(position), centre);
}


HexOffsetCoord operator+(HexOffsetCoord a, HexOffsetCoord b) noexcept
{
	return HexOffsetCoord(a.x + b.x, a.y + b.y);
}
HexOffsetCoord operator-(HexOffsetCoord a, HexOffsetCoord b) noexcept
{
	return HexOffsetCoord(a.x - b.x, a.y - b.y);
}



constexpr HexCubeCoord::HexCubeCoord(short qInit, short rInit) noexcept :
	q(qInit),
	r(rInit) {}

short HexCubeCoord::GetMagnitude() const noexcept
{
	return std::max(std::max(abs(q), abs(r)), std::max(abs(r), abs(-q-r)));
}

HexCubeCoord HexCubeCoord::GetRounded(vec3 unrounded) noexcept
{
	glm::ivec3 rounded(
		std::nearbyintf(unrounded.x),
		std::nearbyintf(unrounded.y),
		std::nearbyintf(unrounded.z)
	);
	vec3 diff(
		abs(rounded.x - unrounded.x),
		abs(rounded.y - unrounded.y),
		abs(rounded.z - unrounded.z)
	);

	if (diff.x > diff.y && diff.x > diff.z) rounded.x = -rounded.y - rounded.z;
	else if (diff.y > diff.z) rounded.y = -rounded.x - rounded.z;

	return HexCubeCoord(rounded.x, rounded.y);
}

vec2 HexCubeCoord::ToPos(HexCubeCoord hexCubeCoord) noexcept
{
	float x = 3.0f / 2.0f * hexCubeCoord.q * (float)invSqrt3;
	float y = 1.0f / 2.0f * hexCubeCoord.q + hexCubeCoord.r;
	return vec2(x, y);
}

HexCubeCoord HexCubeCoord::GetFromPos(vec2 position) noexcept
{
	vec2 qr = GetFromPosPartial(position);
	float s = -qr.x - qr.y;

	return GetRounded(vec3(qr.x, qr.y, s));
}

float HexCubeCoord::GetMagnitudePartial(vec2 partialCubeCoord) noexcept
{
	const float& q = partialCubeCoord.x;
	const float& r = partialCubeCoord.y;
	return std::max(std::max(abs(q), abs(r)), std::max(abs(r), abs(-q - r)));
}
vec2 HexCubeCoord::GetFromPosPartial(vec2 position) noexcept
{
	float q = 2.0f / 3.0f * position.x * (float)sqrt3;
	float r = position.y - q / 2;
	return vec2(q, r);
}

HexCubeCoord HexCubeCoord::operator+=(HexCubeCoord other) noexcept
{
	q += other.q;
	r += other.r;

	return *this;
}


HexCubeCoord operator+(HexCubeCoord a, HexCubeCoord b) noexcept
{
	return HexCubeCoord(a.q + b.q, a.r + b.r);
}
HexCubeCoord operator-(HexCubeCoord a, HexCubeCoord b) noexcept
{
	return HexCubeCoord(a.q - b.q, a.r - b.r);
}



HexOffsetCoord HexCubeToOffset(HexCubeCoord cubeCoords, HexOffsetCoord centre) noexcept
{
	/// (cubeCoords.q & 1) returns 0 if even, 1 if odd. More Info: https://en.wikipedia.org/wiki/Two's_complement
	short x = cubeCoords.q;
	short y = cubeCoords.r + (cubeCoords.q - (cubeCoords.q & 1)) / 2;
	return HexOffsetCoord(x, y) + centre;
}
HexCubeCoord HexOffsetToCube(HexOffsetCoord offsetCoords, HexOffsetCoord centre) noexcept
{
	/// (offsetCoords.x & 1) returns 0 if even, 1 if odd. More Info: https://en.wikipedia.org/wiki/Two's_complement
	offsetCoords = offsetCoords - centre;
	short q = offsetCoords.x;
	short r = offsetCoords.y - (offsetCoords.x - (offsetCoords.x & 1)) / 2;
	return HexCubeCoord(q, r);
}
