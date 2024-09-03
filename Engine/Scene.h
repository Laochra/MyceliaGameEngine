#pragma once

#include "JsonIncludes.h"
#include <string>
using std::string;

namespace Scene
{
	typedef unsigned int uint;

	extern string currentPath;
	extern const uint currentFormatVersion;
	extern bool dirty;

	extern void Clear() noexcept;
	extern void NewScene() noexcept;
	extern bool Open(const char* filepath) noexcept;
	extern bool Load(json scene) noexcept;
	extern bool Save() noexcept;
}
