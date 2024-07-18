#pragma once

#include "GUI.h"

#include "MathIncludes.h"

namespace SceneGUI
{
	extern vec2 normalisedMousePos;

	extern void DrawScene(const char* const name, bool& open) noexcept;
	extern void DrawFileDropdown() noexcept;
	extern void CheckForFileShortcuts() noexcept;

	extern bool EnsureClearIsIntentional() noexcept;

	extern bool NewScene() noexcept;
	extern bool OpenScene() noexcept;
	extern bool SaveScene() noexcept;
	extern bool SaveSceneAs() noexcept;
}
