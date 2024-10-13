#pragma once

#include "InputCode.h"

#include <vector>
#include <string>

typedef unsigned char ubyte;

enum class GamepadType : ubyte
{
	Undefined,
	Xbox,
	Playstation
};

inline constexpr GamepadType GetTypeFromDeviceName(const char* const name) noexcept
{
	GamepadType type;

	if (name == std::string("Xbox Controller")) type = GamepadType::Xbox;
	else if (name == std::string("DualSense Wireless Controller")) type = GamepadType::Playstation;
	else type = GamepadType::Undefined;

	return type;
}
