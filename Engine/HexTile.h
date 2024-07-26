#pragma once

#include "MathIncludes.h"
#include <unordered_map>
#include <vector>

#include "MeshRenderer.h"

enum class HexDir : unsigned char { NorthWest, North, NorthEast, SouthEast, South, SouthWest };

class HexTile : public MeshRenderer
{
public:
	SerialiseAs(HexTile)

	using MeshRenderer::MeshRenderer;

	HexTile* adjacent[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, };

	virtual void Initialise() override;
	void DrawHexPos() noexcept;
	
	HexTile*& operator[](HexDir direction) noexcept;

	const static std::vector<vec3> DirVec;
	static HexDir OppositeDir(HexDir dir);
};
