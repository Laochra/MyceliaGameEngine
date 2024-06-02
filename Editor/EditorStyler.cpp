#include "EditorStyler.h"

namespace EditorStyler
{
	using namespace GUI;

	bool dirty = false;

	void EditorStyler::Draw(const char* const name, bool& open) noexcept
	{
		ImGui::Begin(name, &open);

		ImGui::Text("Current Style");
		if (ImGui::BeginCombo("##Current Style", currentStyle.c_str()))
		{
			bool isCurrent;
			for (pair<string, string> stylePair : styleMap)
			{
				isCurrent = stylePair.first == currentStyle;
				if (ImGui::Selectable(stylePair.first.c_str(), stylePair.first == currentStyle))
				{
					LoadStyle(stylePair.first);
				}
				if (isCurrent) ImGui::SetItemDefaultFocus();
			}

			isCurrent = currentStyle == "Custom";
			if (ImGui::Selectable("Custom", isCurrent))
			{
				LoadStyle("Custom");
			}
			if (isCurrent) ImGui::SetItemDefaultFocus();

			ImGui::EndCombo();
		}

		if (currentStyle == "Custom")
		{
			ImGuiColorEditFlags flags =
				ImGuiColorEditFlags_AlphaBar |
				ImGuiColorEditFlags_AlphaPreview |
				ImGuiColorEditFlags_NoInputs;


			//ImGui::BeginDisabled(!dirty);
			if (ImGui::Button("Save"))
			{
				SaveStyle();
			}
			ImGui::SameLine();
			if (ImGui::Button("Revert"))
			{
				LoadStyle(currentStyle);
			}
			//ImGui::EndDisabled();

			bool changeMade = false;

			ImGui::PushID("Text"); ImGui::SeparatorText("Text");
			DrawColourEdit3("Regular", colours.text, &changeMade, flags);
			DrawColourEdit3("Disabled", colours.textDisabled, &changeMade, flags);
			DrawColourEdit3("Selected", colours.textSelection, &changeMade, flags);
			ImGui::PopID(); GUI::Spacing(3);

			ImGui::PushID("Backgrounds"); ImGui::SeparatorText("Backgrounds");
			DrawColourEdit3("Main", colours.background, &changeMade, flags);
			DrawColourEdit3("Active", colours.backgroundActive, &changeMade, flags);
			DrawColourEdit3("Accent", colours.accentBackground, &changeMade, flags);
			ImGui::PopID(); GUI::Spacing(3);

			ImGui::PushID("Input Fields"); ImGui::SeparatorText("Input Fields");
			DrawColourEdit3("Regular", colours.inputBoxes, &changeMade, flags);
			DrawColourEdit3("Hovered", colours.inputBoxesHover, &changeMade, flags);
			DrawColourEdit3("Clicked", colours.inputBoxesClick, &changeMade, flags);
			DrawColourEdit3("Toggled", colours.toggledInput, &changeMade, flags);
			ImGui::PopID(); GUI::Spacing(3);

			ImGui::PushID("Buttons & Resize"); ImGui::SeparatorText("Buttons & Resize");
			DrawColourEdit3("Regular", colours.interactable, &changeMade, flags);
			DrawColourEdit3("Hovered", colours.interactableHover, &changeMade, flags);
			DrawColourEdit3("Clicked", colours.interactableClick, &changeMade, flags);
			ImGui::PopID(); GUI::Spacing(3);

			ImGui::PushID("Foldable Headers"); ImGui::SeparatorText("Foldable Headers");
			DrawColourEdit3("Regular", colours.header, &changeMade, flags);
			DrawColourEdit3("Hovered", colours.headerHover, &changeMade, flags);
			DrawColourEdit3("Clicked", colours.headerClick, &changeMade, flags);
			ImGui::PopID(); GUI::Spacing(3);

			ImGui::PushID("Tabs"); ImGui::SeparatorText("Tabs");
			DrawColourEdit3("Regular", colours.tab, &changeMade, flags);
			DrawColourEdit3("Hovered", colours.tabHover, &changeMade, flags);
			DrawColourEdit3("Active", colours.tabActive, &changeMade, flags);
			DrawColourEdit3("Unfocused", colours.tabUnfocused, &changeMade, flags);
			DrawColourEdit3("Unfocused & Active", colours.tabUnfocusedActive, &changeMade, flags);
			ImGui::PopID(); GUI::Spacing(3);

			ImGui::PushID("Scrollbar Handles"); ImGui::SeparatorText("Scrollbars");
			DrawColourEdit3("Regular", colours.scrollHandle, &changeMade, flags);
			DrawColourEdit3("Hovered", colours.scrollHandleHover, &changeMade, flags);
			DrawColourEdit3("Clicked", colours.scrollHandleClick, &changeMade, flags);
			ImGui::PopID(); GUI::Spacing(3);

			ImGui::PushID("Borders & Bars"); ImGui::SeparatorText("Borders & Bars");
			DrawColourEdit3("Seperator Bar", colours.seperatorBar, &changeMade, flags);
			DrawColourEdit3("Window Border", colours.windowBorder, &changeMade, flags);
			ImGui::PopID(); GUI::Spacing(3);

			ImGui::PushID("Tables"); ImGui::SeparatorText("Tables");
			DrawColourEdit3("Main Border", colours.tableBorderMain, &changeMade, flags);
			DrawColourEdit3("Inner Border", colours.tableBorderInner, &changeMade, flags);
			DrawColourEdit3("Row Fill", colours.tableRowFill, &changeMade, flags);
			DrawColourEdit3("Alt Row Fill", colours.tableAltRowFill, &changeMade, flags);
			ImGui::PopID(); GUI::Spacing(3);

			ImGui::PushID("Docking"); ImGui::SeparatorText("Docking");
			DrawColourEdit3("Preview", colours.docking, &changeMade, flags);
			DrawColourEdit3("Empty Fill", colours.emptyDockingFill, &changeMade, flags);
			ImGui::PopID();

			if (changeMade)
			{
				GUI::UpdateStyle();
			}
		}

		ImGui::End();
	}

	void EditorStyler::DrawColourEdit3(const char* name, GUI::Colour& colour, bool* changeMade, ImGuiColorEditFlags flags) noexcept
	{
		*changeMade = ImGui::ColorEdit3(name, colour, flags) || *changeMade;
	}
	void EditorStyler::DrawColourEdit4(const char* name, GUI::Colour& colour, bool* changeMade, ImGuiColorEditFlags flags) noexcept
	{
		*changeMade = ImGui::ColorEdit4(name, colour, flags) || *changeMade;
	}
}