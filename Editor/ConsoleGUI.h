#pragma once

#include "GUI.h"

#include "ImGuiIncludes.h"

#include <vector>
#include <iostream>
#include <string>
using std::string;
#include <initializer_list>
using stringParams = std::initializer_list<string>;

namespace ConsoleGUI
{
	enum LogType { Message, Warning, Error };

	struct ConsoleLog
	{
		LogType type;
		string message;

		ConsoleLog(LogType typeInit, string messageInit) : type(typeInit), message(messageInit) { }
	};
	inline std::vector<ConsoleLog> logs;

	inline void Draw();
}

using ConsoleGUI::LogType;

inline void Log(stringParams message, LogType type = LogType::Message);
inline void ClearLogs();


void ConsoleGUI::Draw()
{
	if (ImGui::Button("Clear"))
	{
		ClearLogs();
	}

	ImGui::BeginChild("Logs");
		for (int i = (int)logs.size() - 1; i >= 0; i--)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, logs[i].type == Message ? IM_COL32(255, 255, 255, 255)
				: logs[i].type == Warning ? IM_COL32(255, 255, 0, 255)
				: IM_COL32(255, 0, 0, 255));
			ImGui::TextWrapped(logs[i].message.c_str());
			ImGui::PopStyleColor();
		}
	ImGui::EndChild();
}


void Log(stringParams message, LogType type)
{
	string logMessage = string(std::to_string(ConsoleGUI::logs.size())) + string(": ");
	for (string str : message) { logMessage += str; }

	ConsoleGUI::logs.push_back(ConsoleGUI::ConsoleLog(type, logMessage));

	#if _DEBUG
	const char* typeMessage = type == LogType::Message ? "\nMessage "
		: type == LogType::Warning ? "\nWarning: "
		: "\nError: ";
	std::cout << typeMessage << logMessage << "\n";
	#endif
}

void ClearLogs()
{
	ConsoleGUI::logs.clear();
}