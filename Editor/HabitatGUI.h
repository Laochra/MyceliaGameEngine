#pragma once

#include "ImGuiIncludes.h"
#include "MeshRendererGUI.h"

#include "HabitatObject.h"

class HabitatGUI : public MeshRendererGUI
{
public:
	using MeshRendererGUI::MeshRendererGUI;

	virtual void Draw() override;

protected:
	static void DrawHabitatGUI(HabitatObject* habitat);
};
