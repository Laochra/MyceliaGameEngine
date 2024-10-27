#pragma once

#include "GUI.h"
#include "TileData.h"
#include "HexType.h"

typedef unsigned int uint;

class UISprite;

class HexEditor
{
public:
	static vector<TileData>* selectedType;
	static uint selectedVariant;

	static void Draw(const char* const name, bool& open) noexcept;

private:
	static void DrawType(vector<TileData>& type) noexcept;
	static bool DrawPrefabInput(const char* const name, string& prefabFilepath) noexcept;
	static void DrawSpriteInput(const char* const name, string& spriteFilepath) noexcept;
	static bool DrawHDRTextureInput(const char* const name, string& hdrTextureFilepath) noexcept;
	static void DrawAudioInput(const char* const name, string& audioFilepath) noexcept;
	static void DrawTileDropdown(const char* label, string& tileName) noexcept;
	static void DrawUISpriteDropdown(const char* label, UISprite*& uiSprite) noexcept;
};
