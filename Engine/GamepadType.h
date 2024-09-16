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

	if (name == std::string("XInput Gamepad (GLFW)")) type = GamepadType::Xbox;
	else if (name == std::string("PS4 Controller")) type = GamepadType::Playstation;
	else type = GamepadType::Undefined;

	return type;
}

inline const std::vector<InputCode> gamepadLayoutXbox =
{
	InputCode::GamepadA,
	InputCode::GamepadB,
	InputCode::GamepadX,
	InputCode::GamepadY,
	InputCode::GamepadLB,
	InputCode::GamepadRB,
	InputCode::GamepadLM,
	InputCode::GamepadRM,
	InputCode::GamepadLS,
	InputCode::GamepadRS,
	InputCode::GamepadDPUp,
	InputCode::GamepadDPRight,
	InputCode::GamepadDPDown,
	InputCode::GamepadDPLeft,

	InputCode::GamepadLSX,
	InputCode::GamepadLSY,
	InputCode::GamepadRSX,
	InputCode::GamepadRSY,
	InputCode::GamepadLT,
	InputCode::GamepadRT,
};
inline const std::vector<InputCode> gamepadLayoutPlaystation =
{
	InputCode::GamepadX,
	InputCode::GamepadA,
	InputCode::GamepadB,
	InputCode::GamepadY,
	InputCode::GamepadLB,
	InputCode::GamepadRB,
	InputCode::GamepadUndefined,
	InputCode::GamepadUndefined,
	InputCode::GamepadUndefined,
	InputCode::GamepadRM,
	InputCode::GamepadLS,
	InputCode::GamepadRS,
	InputCode::GamepadUndefined,
	InputCode::GamepadLM,
	InputCode::GamepadUndefined,
	InputCode::GamepadDPUp,
	InputCode::GamepadDPRight,
	InputCode::GamepadDPDown,
	InputCode::GamepadDPLeft,

	InputCode::GamepadLSX,
	InputCode::GamepadLSY,
	InputCode::GamepadRSX,
	InputCode::GamepadLT,
	InputCode::GamepadRT,
	InputCode::GamepadRSY,
};

inline constexpr std::vector<InputCode> GetGamepadLayout(GamepadType gamepadType) noexcept
{
	switch (gamepadType)
	{
	case GamepadType::Xbox:					return gamepadLayoutXbox;
	case GamepadType::Playstation:		return gamepadLayoutPlaystation;
	default:										return gamepadLayoutXbox;
	}
}
