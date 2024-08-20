#pragma once

#include "GUI.h"

typedef unsigned int uint;

enum TileType : uint
{
	Tree,
	Flower,
	Water,
	Land
};

struct TileData
{
	string name = "New Variant";
	string prefabFilepaths[3] = { "None", "None", "None" };

	TileData() noexcept = default;
	TileData(const char* const nameInit) noexcept : name(nameInit) {}
};

class TileEditor
{
public:
	static uint selectionIndices[2];
	static vector<vector<TileData>> variants;

	static void Draw(const char* const name, bool& open) noexcept;
	
private:
	static void DrawType(TileType type) noexcept;
	static void DrawPrefabInput(const char* const name, string& prefabFilepath);
	static TileData& SelectedTileData() noexcept;
};
