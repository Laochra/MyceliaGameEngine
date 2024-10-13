#include "GamepadInfo.h"

#include "Input.h"

GamepadInfo::GamepadInfo(uint gamepadIDInit) noexcept :
	enabled(true),
	gamepadType(GamepadType::Undefined) { }

bool GamepadInfo::IsEnabled() const noexcept
{
	return enabled;
}

void GamepadInfo::Connect(GamepadType gamepadTypeInit) noexcept
{
	enabled = true;
	gamepadType = gamepadTypeInit;
}
void GamepadInfo::Disconnect() noexcept
{
	enabled = false;
	gamepadType = GamepadType::Undefined;
}

uint GamepadInfo::GetGamepadID() const noexcept
{
	return gamepadID;
}

GamepadType GamepadInfo::GetGamepadType() const noexcept
{
	return gamepadType;
}

InputCode GamepadInfo::GetGlobalInputCode(uint index) const noexcept
{
	return InputCode(uint(InputCode::GamepadFirstButton) + index);
}

InputCode GamepadInfo::GetLocalInputCode(uint index) const noexcept
{
	return InputCode(uint(InputCode::GamepadFirstButton) + index + ((gamepadID + 1) * 100));
}
