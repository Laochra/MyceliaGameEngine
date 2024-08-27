#pragma once

#include "GUI.h"
#include "HexTile.h"

typedef unsigned int uint;

enum TileType : uint
{
	Tree,
	Flower,
	Water,
	Land
};

class TileEditor
{
public:
	static vector<TileData>* selectedType;
	static uint selectedVariant;

	static void Draw(const char* const name, bool& open) noexcept;

private:
	static void DrawType(vector<TileData>& type) noexcept;
	static void DrawPrefabInput(const char* const name, string& prefabFilepath);
	static void DrawSpriteInput(const char* const name, string& spriteFilepath);
};
