#pragma once

#include "Input.h"
#include "ImGuiIncludes.h"

#include <map>

namespace InputEditor
{
	extern void ConfigureInputBind(const char* label, InputBind* inputBind);
	extern void ConfigureInputAxis(const char* negativeLabel, const char* positiveLabel, InputAxis* inputAxis);
}