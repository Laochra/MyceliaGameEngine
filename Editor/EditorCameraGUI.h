#pragma once

#include "ImGuiIncludes.h"

#include "EditorCamera.h"
#include "EditorCameraConfig.h"
#include "InputEditor.h"

class EditorCameraGUI
{
public:
	static void DrawEditorCameraGUI(EditorCamera* camera);
};