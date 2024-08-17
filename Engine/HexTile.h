#pragma once

#include "MathIncludes.h"
#include <unordered_map>
#include <vector>

#include "MeshRenderer.h"

class Habitat;

enum class HexDir : unsigned char { NorthWest, North, NorthEast, SouthEast, South, SouthWest };
enum class HexType { Available, Grass, Water, Flower };

class HexTile : public MeshRenderer
{
public:
	SerialiseAs(HexTile)

	using MeshRenderer::MeshRenderer;

	Habitat* habitat = nullptr;
	HexTile* adjacent[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, };

	HexType type = HexType::Available;

	virtual void Initialise() override;
	void DrawHexPos() noexcept;
	
	glm::ivec2 GetHexPos() const noexcept;

	HexTile*& operator[](HexDir direction) noexcept;

	const static std::vector<glm::ivec2> DirVec;
	static vec3 HexPosToRealPos(glm::ivec2 hexPos);
	static glm::ivec2 RealPosToHexPos(vec3 hexPos);
	static HexDir OppositeDir(HexDir dir);

	static json availablePrefab;
	static json grassPrefab;
	static json waterPrefab;
	static json flowerPrefab;
};
