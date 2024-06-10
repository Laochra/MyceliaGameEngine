#pragma once

#include "JsonIncludes.h"

namespace Scene
{
	typedef unsigned int uint;

	extern const char* currentPath;
	extern const uint currentFormatVersion;
	extern bool dirty;

	extern void Clear() noexcept;
	extern bool Open(const char* filepath) noexcept;
	extern bool Load(json scene) noexcept;
	extern bool Save() noexcept;
}
