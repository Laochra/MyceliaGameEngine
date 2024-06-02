#pragma once

#include "GUI.h"

typedef unsigned int uint;

namespace MeshCooker
{
	extern vector<string> fileQueue;
	extern uint current;

	extern void Draw() noexcept;
	extern void Cook(const char* filepath) noexcept;
}