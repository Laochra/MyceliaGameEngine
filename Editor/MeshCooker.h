#pragma once

#include "GUI.h"

typedef unsigned int uint;

namespace MeshCooker
{
	extern vector<string> fileQueue;
	extern uint current;

	extern void Draw(const char* const name, bool& open) noexcept;
	extern void Cook(const char* filepath) noexcept;
}