#pragma once

#include "GUI.h"

#include "ImGuiIncludes.h"

#include <vector>

#include "EditorDebug.h"

namespace ConsoleGUI
{
	inline std::vector<Debug::DebugLog> logs;

	inline void Draw();
}


void ConsoleGUI::Draw()
{
	if (ImGui::Button("Clear"))
	{
		logs.clear();
	}

	ImGui::BeginChild("Logs");
		ImGuiListClipper clipper; // Only draw the logs that would appear on screen
		clipper.Begin((int)logs.size());
		while (clipper.Step())
		{
			for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, logs[i].type == Debug::Message ? IM_COL32(255, 255, 255, 255)
					: logs[i].type == Debug::Warning ? IM_COL32(255, 255, 0, 255)
					: IM_COL32(255, 0, 0, 255));
				ImGui::TextWrapped((std::to_string(i) + ". " + Debug::GetLogAsString(logs[i])).c_str());
				ImGui::PopStyleColor();
			}
		}
		clipper.End();

		if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) ImGui::SetScrollHereY(1.0f); // If at bottom, autoscroll down in case a new log was added
	ImGui::EndChild();
}