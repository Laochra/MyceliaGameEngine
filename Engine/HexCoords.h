#pragma once

constexpr double sqrt3 = 1.7320508075689;
constexpr double invSqrt3 = 1 / 1.7320508075689;

#include "HexDirection.h"

#include "MathIncludes.h"
#include <vector>

struct HexOffsetCoord
{
	short x, y;

	HexOffsetCoord(short xInit = 0, short yInit = 0) noexcept;

	static const HexOffsetCoord neighbours[12];

	short GetMagnitude(HexOffsetCoord centre) const noexcept;
	static HexOffsetCoord GetCoords(HexOffsetCoord hexCoord, HexDirection direction) noexcept;

	static vec2 ToPos(HexOffsetCoord hexOffsetCoord, HexOffsetCoord centre) noexcept;
	static HexOffsetCoord GetFromPos(vec2 position, HexOffsetCoord centre) noexcept;

	bool operator==(const HexOffsetCoord& other) const noexcept = default;
	auto operator<=>(const HexOffsetCoord& other) const noexcept = default;
};
HexOffsetCoord operator+(HexOffsetCoord a, HexOffsetCoord b) noexcept;
HexOffsetCoord operator-(HexOffsetCoord a, HexOffsetCoord b) noexcept;


struct HexCubeCoord
{
	short q, r;

	constexpr HexCubeCoord(short qInit = 0, short rInit = 0) noexcept;

	short GetMagnitude() const noexcept;

	static HexCubeCoord GetRounded(vec3 unrounded) noexcept;

	static vec2 ToPos(HexCubeCoord hexCubeCoord) noexcept;
	static HexCubeCoord GetFromPos(vec2 position) noexcept;

	static float GetMagnitudePartial(vec2 partialCubeCoord) noexcept;
	static vec2 GetFromPosPartial(vec2 position) noexcept;

	HexCubeCoord operator+=(HexCubeCoord other) noexcept;

	bool operator==(const HexCubeCoord& other) const noexcept = default;
	auto operator<=>(const HexCubeCoord& other) const noexcept = default;

	static std::vector<HexCubeCoord> GetCoordsWithRange(short range) noexcept;
};
HexCubeCoord operator+(HexCubeCoord a, HexCubeCoord b) noexcept;
HexCubeCoord operator-(HexCubeCoord a, HexCubeCoord b) noexcept;


extern HexOffsetCoord HexCubeToOffset(HexCubeCoord cubeCoords, HexOffsetCoord centre) noexcept;
extern HexCubeCoord HexOffsetToCube(HexOffsetCoord offsetCoords, HexOffsetCoord centre) noexcept;
