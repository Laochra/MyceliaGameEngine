#pragma once

#include "GUI.h"

namespace SceneGUI
{
	extern void DrawScene(const char* const name, bool& open) noexcept;
	extern void DrawFileDropdown() noexcept;

	extern bool EnsureClearIsIntentional() noexcept;

	extern bool NewScene() noexcept;
	extern bool OpenScene() noexcept;
	extern bool SaveScene() noexcept;
	extern bool SaveSceneAs() noexcept;
}
