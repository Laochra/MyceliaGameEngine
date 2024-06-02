#pragma once

#include "GUI.h"
#include "JsonIncludes.h"

typedef unsigned int uint;

namespace SceneGUI
{
	extern const char* currentScenePath;
	extern const uint currentFormatVersion;
	extern bool dirty;

	extern void DrawScene() noexcept;
	extern void DrawFileDropdown() noexcept;

	extern bool NewScene() noexcept;
	extern bool OpenScene() noexcept;
	extern bool SaveScene() noexcept;
	extern bool SaveSceneAs() noexcept;

	extern bool ClearScene() noexcept;
	extern bool LoadScene(json scene) noexcept;
}