#pragma once

#include "ImGuiIncludes.h"
#include "GameObject3DGUI.h"

#include "LinkedHexGrid.h"

class LinkedHexGridGUI : public GameObject3DGUI
{
public:
	using GameObject3DGUI::GameObject3DGUI;

	virtual void Draw() override;

protected:
	static void DrawLinkedHexGridGUI(LinkedHexGrid* linkedHexGrid);
};