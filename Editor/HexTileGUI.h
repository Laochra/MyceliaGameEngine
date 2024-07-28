#pragma once

#include "ImGuiIncludes.h"
#include "MeshRendererGUI.h"

#include "HexTile.h"

class HexTileGUI : public MeshRendererGUI
{
public:
	using MeshRendererGUI::MeshRendererGUI;

	virtual void Draw() override;

protected:
	static void DrawHexTileGUI(HexTile* hexTile);
};