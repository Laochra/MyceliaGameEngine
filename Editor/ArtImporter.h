#pragma once

#include "GUI.h"

typedef unsigned int uint;

namespace ArtImporter
{
	extern vector<string> fileQueue;
	extern uint current;

	extern void Draw(const char* const name, bool& open) noexcept;
	extern void Add(const string filepath) noexcept;
	extern void Preload(const char* filepath) noexcept;
	extern void CookCurrent() noexcept;
	extern void CookAll() noexcept;
}