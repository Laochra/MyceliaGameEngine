#pragma once

#include "MathIncludes.h"
#include <unordered_map>
#include <vector>

#include "MeshRenderer.h"

class Habitat;

enum class HexDir : unsigned char { NorthWest, North, NorthEast, SouthEast, South, SouthWest };
enum class HexType { Empty, Tree, Flower, Water, Land };
struct TileData
{
	string name = "New Variant";
	string prefabFilepaths[3] = { "None", "None", "None" };

	TileData() noexcept = default;
	TileData(const char* const nameInit) noexcept : name(nameInit) {}
};

class HexTile : public MeshRenderer
{
public:
	SerialiseAs(HexTile)

	using MeshRenderer::MeshRenderer;

	Habitat* habitat = nullptr;
	HexTile* adjacent[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, };

	HexType type = HexType::Empty;
	string variant = "Empty";

	virtual void Initialise() override;
	void DrawHexPos() noexcept;
	
	glm::ivec2 GetHexPos() const noexcept;

	HexTile*& operator[](HexDir direction) noexcept;

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

	static json GetEmptyTilePrefab() noexcept;
	static json GetDefaultTilePrefab() noexcept;
	static json GetTilePrefab(string name, uint density) noexcept;

private:
	static void AddDefaultPrefab(string name, string path) noexcept;
	static void AddTilePrefab(string name, uint density) noexcept;
	static map<string, json> prefabs;
};
