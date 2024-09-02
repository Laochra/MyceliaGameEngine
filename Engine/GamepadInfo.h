#pragma once

#include "InputCode.h"
#include "GamepadType.h"

#include <vector>
#include <string>

typedef unsigned int uint;

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
	uint gamepadID;
	GamepadType gamepadType;
	const std::vector<InputCode>* layout;
};
