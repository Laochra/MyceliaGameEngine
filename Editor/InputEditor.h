#pragma once

#include "Input.h"
#include "ImGuiIncludes.h"

#include <map>

class InputEditor
{
public:
	std::map<KeyCode, const char*>* keyCodeToName;
	std::map<const char*, KeyCode>* keyNameToCode;

	InputEditor() noexcept;

	void ConfigureKeybind(const char* label, Keybind* keybind);

	void ConfigureKeyAxis(const char* negativeLabel, const char* positiveLabel, KeyAxis* keyAxis);
};