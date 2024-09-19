#pragma once

#include "ImGuiIncludes.h"
#include "MeshRendererGUI.h"

#include "HexTileObject.h"

class HexTileGUI : public MeshRendererGUI
{
public:
	using MeshRendererGUI::MeshRendererGUI;

	virtual void Draw() override;

protected:
	static void DrawHexTileGUI(HexTileObject* hexTile);
};