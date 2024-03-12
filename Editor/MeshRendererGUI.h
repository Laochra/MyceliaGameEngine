#pragma once

#include "ImGuiIncludes.h"
#include "GameObject3DGUI.h"

#include "MeshRenderer.h"

class MeshRendererGUI : public GameObject3DGUI
{
public:
	using GameObject3DGUI::GameObject3DGUI;

	virtual void Draw() override;

protected:
	static void DrawMeshRendererGUI(MeshRenderer* meshRenderer);
};