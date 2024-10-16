#pragma once

#include "ImGuiIncludes.h"
#include "GameObject3DGUI.h"

#include "HexGrid.h"

class HexGridGUI : public GameObject3DGUI
{
public:
	using GameObject3DGUI::GameObject3DGUI;

	virtual void Draw() override;

protected:
	static void DrawHexGridGUI(HexGrid* hexGrid);
};