#pragma once

#include "GUI.h"

#include "ImGuiIncludes.h"

#include <vector>

#include "EditorDebug.h"

namespace ConsoleGUI
{
	extern std::vector<Debug::DebugLog> logs;

	extern void Draw(const char* const name, bool& open);
}