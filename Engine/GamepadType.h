#pragma once

#include "InputCode.h"

#include <vector>

enum class GamepadType
{
	Undefined,
	Xbox,
	Playstation
};

inline constexpr GamepadType GetTypeFromDeviceName(const char* const name) noexcept
{
	GamepadType type;

	if (name == string("Xbox Controller")) type = GamepadType::Xbox;
	else if (name == string("DualSense Wireless Controller")) type = GamepadType::Playstation;
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

	InputCode::GamepadLT,
	InputCode::GamepadRT,
	InputCode::GamepadLSX,
	InputCode::GamepadLSY,
	InputCode::GamepadRSX,
	InputCode::GamepadRSY,
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
	InputCode::GamepadLM,
	InputCode::GamepadRM,
	InputCode::GamepadLS,
	InputCode::GamepadRS,
	InputCode::GamepadUndefined,
	InputCode::GamepadUndefined,
	InputCode::GamepadUndefined,
	InputCode::GamepadDPUp,
	InputCode::GamepadDPRight,
	InputCode::GamepadDPDown,
	InputCode::GamepadDPLeft,

	InputCode::GamepadLT,
	InputCode::GamepadRT,
	InputCode::GamepadLSX,
	InputCode::GamepadLSY,
	InputCode::GamepadRSX,
	InputCode::GamepadRSY,
};

inline constexpr const std::vector<InputCode>* GetGamepadLayout(GamepadType gamepadType) noexcept
{
	switch (gamepadType)
	{
	case GamepadType::Xbox: return &gamepadLayoutXbox;
	case GamepadType::Playstation: return &gamepadLayoutPlaystation;
	default: return &gamepadLayoutXbox;
	}
}
