#pragma once

#include "InputCode.h"
#include "GamepadType.h"

#include <vector>
#include <string>

typedef unsigned int uint;
typedef unsigned char ubyte;

class GamepadInfo
{
public:
	GamepadInfo(uint gamepadIDInit) noexcept;
	
	bool IsEnabled() const noexcept;

	void Connect(GamepadType gamepadTypeInit = GamepadType::Undefined) noexcept;
	void Disconnect() noexcept;

	uint GetGamepadID() const noexcept;
	GamepadType GetGamepadType() const noexcept;

	InputCode GetGlobalInputCode(uint index) const noexcept;
	InputCode GetLocalInputCode(uint index) const noexcept;

private:
	bool enabled;
	ubyte gamepadID;
	GamepadType gamepadType;
};
