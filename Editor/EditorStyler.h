#pragma once

#include "GUI.h"

namespace EditorStyler
{
	using namespace GUI;

	extern bool dirty;

	extern void Draw(const char* const name, bool& open) noexcept;

	extern void DrawColourEdit3(const char* name, GUI::Colour& colour, bool* changeMade, ImGuiColorEditFlags flags) noexcept;
	extern void DrawColourEdit4(const char* name, GUI::Colour& colour, bool* changeMade, ImGuiColorEditFlags flags) noexcept;
}