#pragma once

#include "HexType.h"
#include "TileData.h"

#include "MathIncludes.h"
#include <unordered_map>
#include <vector>

#include "MeshRenderer.h"

class HabitatObject;

typedef unsigned int uint;
typedef unsigned char ubyte;

enum class HexDir : ubyte { NorthWest, North, NorthEast, SouthEast, South, SouthWest };

class HexTileObject : public MeshRenderer
{
public:
	SerialiseAs(HexTileObject)

	using MeshRenderer::MeshRenderer;

	HabitatObject* habitat = nullptr;
	HexTileObject* adjacent[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, };

	HexType type = HexType::Perimeter;
	string variant = "Empty";

	virtual void Initialise() override;
	void DrawHexPos() noexcept;
	void DrawFogMasks() noexcept;
	
	glm::ivec2 GetHexPos() const noexcept;

	HexTileObject*& operator[](HexDir direction) noexcept;

	const static std::vector<glm::ivec2> DirVec;
	static vec3 HexPosToRealPos(glm::ivec2 hexPos);
	static glm::ivec2 RealPosToHexPos(vec3 hexPos);
	static HexDir OppositeDir(HexDir dir);

	static void LoadTileDataFrom(json& tilesFile) noexcept;
	static void SaveTileDataTo(json& tilesFile) noexcept;

	static string emptyTilePath;
	static string defaultTilePath;
	static vector<TileData> trees;
	static vector<TileData> flowers;
	static vector<TileData> waters;
	static vector<TileData> lands;

	static string treeRadialSprites[3];
	static string flowerRadialSprites[3];
	static string waterRadialSprites[3];
	static string landRadialSprites[3];

	static json GetEmptyTilePrefab() noexcept;
	static json GetDefaultTilePrefab() noexcept;
	static json GetTilePrefab(string name) noexcept;
	static void ClearPrefabs() noexcept;

private:
	static void AddDefaultPrefab(string name, string path) noexcept;
	static void AddTilePrefab(string name) noexcept;
	static map<string, json> prefabs;
};
