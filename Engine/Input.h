#pragma once

#include "GLIncludes.h"
#include <set>
#include <map>
#include <vector>

#include "InputCode.h"
#include "GamepadInfo.h"

class Input
{
public:
	bool enabled = true;

	std::vector<GamepadInfo> gamepads;

	std::set<InputCode> downInputs;
	std::set<InputCode> pressedInputs;
	std::set<InputCode> releasedInputs;

	std::map<InputCode, float> axisInputs;
	float axisDeadzone = 0.003f;

	vec2 cursorPos = vec2(0, 0);
	// This acts differently based on the application, if the game view exists inside a sub-window this will need to be set relative to that
	vec2 screenCursorPos = vec2(0, 0);
	vec2 cursorMovement = vec2(0, 0);
	vec2 scrollInput = vec2(0, 0);

	Input() noexcept;
	Input(const Input& other) = delete;
	Input& operator=(const Input& other) = delete;

	bool GetInputDown(InputCode inputCode) const noexcept;
	bool GetInputPressed(InputCode inputCode) const noexcept;
	bool GetInputReleased(InputCode inputCode) const noexcept;

	float GetAxis(InputCode axisInputCode) const noexcept;
	int GetAxis(InputCode negativeInputCode, InputCode positiveInputCode) const noexcept;

	void Update();

	static const char* GetNameFromCode(InputCode inputCode) noexcept;
	static InputCode GetCodeFromName(const char* const inputName) noexcept;
};

extern const char* inputs[];

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void ScrollCallback(GLFWwindow* window, double x, double y);
void CursorPosCallback(GLFWwindow* window, double x, double y);
void GamepadConnectionCallback(int gamepadID, int type);

class InputBind
{
public:
	InputBind() noexcept;
	InputBind(InputCode inputCode) noexcept;
	~InputBind() noexcept;
	InputBind(const InputBind& other) = delete;
	InputBind& operator=(const InputBind& other) = delete;

	bool down() const noexcept;
	bool pressed() const noexcept;
	bool released() const noexcept;
	operator bool() const { return down(); }

	void Bind(InputCode inputCode) noexcept;
	void Bind(std::vector<InputCode> inputCodes) noexcept;
	void Unbind(InputCode inputCode) noexcept;
	void ClearBinds() noexcept;

	std::vector<InputCode> GetBinds() { return *binds; }

private:
	std::vector<InputCode>* binds;
};

class InputAxis
{
public:
	InputAxis() noexcept;
	InputAxis(InputCode negative, InputCode positive) noexcept;
	~InputAxis() noexcept;
	InputAxis(const InputAxis& other) = delete;
	InputAxis& operator=(const InputAxis& other) = delete;

	float value() const noexcept;
	operator float() const { return value(); }

	void BindPositive(InputCode inputCode) noexcept;
	void BindPositive(std::vector<InputCode> inputCodes) noexcept;
	void BindNegative(InputCode inputCode) noexcept;
	void BindNegative(std::vector<InputCode> inputCodes) noexcept;
	void BindPair(InputCode negative, InputCode positive) noexcept;

	void UnbindPositive(InputCode inputCode) noexcept;
	void UnbindNegative(InputCode inputCode) noexcept;

	void ClearPositiveBinds() noexcept;
	void ClearNegativeBinds() noexcept;
	void ClearAllBinds() noexcept;

	std::vector<InputCode> GetPositiveBinds() { return *positiveBinds; }
	std::vector<InputCode> GetNegativeBinds() { return *negativeBinds; }

private:
	std::vector<InputCode>* positiveBinds;
	std::vector<InputCode>* negativeBinds;
};