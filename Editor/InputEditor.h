#pragma once

#include "Input.h"
#include "ImGuiIncludes.h"

#include <map>

namespace InputEditor
{
	extern void ConfigureKeybind(const char* label, Keybind* keybind);
	extern void ConfigureKeyAxis(const char* negativeLabel, const char* positiveLabel, KeyAxis* keyAxis);
}