#pragma once

#include "GUI.h"

namespace EditorStyler
{
	using namespace GUI;

	inline void Draw();

	inline void DrawColourEdit3(const char* name, GUI::Colour& colour, bool* dirty, ImGuiColorEditFlags flags);
	inline void DrawColourEdit4(const char* name, GUI::Colour& colour, bool* dirty, ImGuiColorEditFlags flags);
}

void EditorStyler::Draw()
{
	ImGuiColorEditFlags flags =
		ImGuiColorEditFlags_AlphaBar |
		ImGuiColorEditFlags_AlphaPreview |
		ImGuiColorEditFlags_NoInputs;

	bool dirty = false;

	ImGui::PushID("Text"); ImGui::SeparatorText("Text");
	DrawColourEdit3("Regular", colours.text, &dirty, flags);
	DrawColourEdit3("Disabled", colours.textDisabled, &dirty, flags);
	DrawColourEdit3("Selected", colours.textSelection, &dirty, flags);
	ImGui::PopID(); GUI::Spacing(3);

	ImGui::PushID("Backgrounds"); ImGui::SeparatorText("Backgrounds");
	DrawColourEdit3("Main", colours.background, &dirty, flags);
	DrawColourEdit3("Active", colours.backgroundActive, &dirty, flags);
	DrawColourEdit3("Accent", colours.accentBackground, &dirty, flags);
	ImGui::PopID(); GUI::Spacing(3);

	ImGui::PushID("Input Fields"); ImGui::SeparatorText("Input Fields");
	DrawColourEdit3("Regular", colours.inputBoxes, &dirty, flags);
	DrawColourEdit3("Hovered", colours.inputBoxesHover, &dirty, flags);
	DrawColourEdit3("Clicked", colours.inputBoxesClick, &dirty, flags);
	DrawColourEdit3("Toggled", colours.toggledInput, &dirty, flags);
	ImGui::PopID(); GUI::Spacing(3);

	ImGui::PushID("Buttons & Resize"); ImGui::SeparatorText("Buttons & Resize");
	DrawColourEdit3("Regular", colours.interactable, &dirty, flags);
	DrawColourEdit3("Hovered", colours.interactableHover, &dirty, flags);
	DrawColourEdit3("Clicked", colours.interactableClick, &dirty, flags);
	ImGui::PopID(); GUI::Spacing(3);

	ImGui::PushID("Foldable Headers"); ImGui::SeparatorText("Foldable Headers");
	DrawColourEdit3("Regular", colours.header, &dirty, flags);
	DrawColourEdit3("Hovered", colours.headerHover, &dirty, flags);
	DrawColourEdit3("Clicked", colours.headerClick, &dirty, flags);
	ImGui::PopID(); GUI::Spacing(3);

	ImGui::PushID("Tabs"); ImGui::SeparatorText("Tabs");
	DrawColourEdit3("Regular", colours.tab, &dirty, flags);
	DrawColourEdit3("Hovered", colours.tabHover, &dirty, flags);
	DrawColourEdit3("Active", colours.tabActive, &dirty, flags);
	DrawColourEdit3("Unfocused", colours.tabUnfocused, &dirty, flags);
	DrawColourEdit3("Unfocused & Active", colours.tabUnfocusedActive, &dirty, flags);
	ImGui::PopID(); GUI::Spacing(3);

	ImGui::PushID("Scrollbar Handles"); ImGui::SeparatorText("Scrollbars");
	DrawColourEdit3("Regular", colours.scrollHandle, &dirty, flags);
	DrawColourEdit3("Hovered", colours.scrollHandleHover, &dirty, flags);
	DrawColourEdit3("Clicked", colours.scrollHandleClick, &dirty, flags);
	ImGui::PopID(); GUI::Spacing(3);

	ImGui::PushID("Borders & Bars"); ImGui::SeparatorText("Borders & Bars");
	DrawColourEdit3("Seperator Bar", colours.seperatorBar, &dirty, flags);
	DrawColourEdit3("Window Border", colours.windowBorder, &dirty, flags);
	ImGui::PopID(); GUI::Spacing(3);

	ImGui::PushID("Tables"); ImGui::SeparatorText("Tables");
	DrawColourEdit3("Main Border", colours.tableBorderMain, &dirty, flags);
	DrawColourEdit3("Inner Border", colours.tableBorderInner, &dirty, flags);
	DrawColourEdit3("Row Fill", colours.tableRowFill, &dirty, flags);
	DrawColourEdit3("Alt Row Fill", colours.tableAltRowFill, &dirty, flags);
	ImGui::PopID(); GUI::Spacing(3);

	ImGui::PushID("Docking"); ImGui::SeparatorText("Docking");
	DrawColourEdit3("Preview", colours.docking, &dirty, flags);
	DrawColourEdit3("Empty Fill", colours.emptyDockingFill, &dirty, flags);
	ImGui::PopID();
}

void EditorStyler::DrawColourEdit3(const char* name, GUI::Colour& colour, bool* dirty, ImGuiColorEditFlags flags)
{
	*dirty = ImGui::ColorEdit3(name, colour, flags) || *dirty;
}
void EditorStyler::DrawColourEdit4(const char* name, GUI::Colour& colour, bool* dirty, ImGuiColorEditFlags flags)
{
	*dirty = ImGui::ColorEdit4(name, colour, flags) || *dirty;
}