#pragma once

#include "GUI.h"

namespace EditorStyler
{
	typedef ImVec4 Colour;

	struct EditorColours
	{
		Colour text = { 1.0f, 1.0f, 1.0f, 1.0f };
		Colour textDisabled = { 0.5f, 0.5f, 0.5f, 1.0f };

		Colour windowBorder = { 0.05f, 0.0f, 0.1f, 1.0f };

		Colour inputBoxes = { 0.3f, 0.3f, 0.4f, 1.0f };
		Colour inputBoxesHover = { 0.35f, 0.35f, 0.5f, 1.0f };
		Colour inputBoxesClick = { 0.4f, 0.35f, 0.55f, 1.0f };

		Colour background = { 0.1f, 0.1f, 0.15f, 1.0f }; // TitleBg, TitleBgCollapsed, WindowBg, ChildBg, PopupBg, ResizeGrip
		Colour backgroundActive = { 0.1f, 0.05f, 0.15f, 1.0f }; // TitleBgActive
		Colour altBackground = { 0.05f, 0.0f, 0.1f, 1.0f }; // MenuBarBg, ScrollbarBg

	};
	
	inline EditorColours colours;

	void Draw()
	{
		ImGui::ColorEdit3("Text", (float*)&colours.text, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoInputs);
	}
}