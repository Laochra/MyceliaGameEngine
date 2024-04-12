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
		for (int i = 0; i < (int)logs.size(); i++)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, logs[i].type == Debug::Message ? IM_COL32(255, 255, 255, 255)
				: logs[i].type == Debug::Warning ? IM_COL32(255, 255, 0, 255)
				: IM_COL32(255, 0, 0, 255));
			ImGui::TextWrapped(Debug::GetLogAsString(logs[i]).c_str());
			ImGui::PopStyleColor();
		}
	ImGui::EndChild();
}