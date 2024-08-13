#pragma once

#include "ImGuiIncludes.h"
#include "MeshRendererGUI.h"

#include "Habitat.h"

class HabitatGUI : public MeshRendererGUI
{
public:
	using MeshRendererGUI::MeshRendererGUI;

	virtual void Draw() override;

protected:
	static void DrawHabitatGUI(Habitat* habitat);
};
