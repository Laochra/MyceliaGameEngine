#pragma once

#include "ImGuiIncludes.h"
#include "GameObject3DGUI.h"

#include "Camera.h"

class CameraGUI : public GameObject3DGUI
{
public:
	using GameObject3DGUI::GameObject3DGUI;

	virtual void Draw() override;

protected:
	static void DrawCameraGUI(Camera* camera);
};