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
	float axisDeadzone = 0.15f;

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
	InputBind() noexcept = default;
	InputBind(InputCode inputCode) noexcept;
	InputBind(std::vector<InputCode> inputCodes) noexcept;
	InputBind(const InputBind& other) noexcept;
	InputBind& operator=(const InputBind& other) noexcept;

	bool down() const noexcept;
	bool pressed() const noexcept;
	bool released() const noexcept;
	operator bool() const noexcept;

	void Bind(InputCode inputCode) noexcept;
	void Bind(std::vector<InputCode> inputCodes) noexcept;
	void Unbind(InputCode inputCode) noexcept;
	void ClearBinds() noexcept;

	const std::vector<InputCode>& GetBinds() const noexcept;

private:
	std::vector<InputCode> binds;
};

class InputAxis
{
public:
	InputAxis() noexcept = default;
	InputAxis(InputCode negative, InputCode positive) noexcept;
	InputAxis(InputCode axis) noexcept;
	InputAxis(InputCode negative, InputCode positive, InputCode axis) noexcept;
	InputAxis(std::vector<InputCode> negatives, std::vector<InputCode> positives) noexcept;
	InputAxis(std::vector<InputCode> axes) noexcept;
	InputAxis(std::vector<InputCode> negatives, std::vector<InputCode> positives, std::vector<InputCode> axes) noexcept;
	InputAxis(const InputAxis& other) noexcept;
	InputAxis& operator=(const InputAxis& other) noexcept;

	float value() const noexcept;
	operator float() const noexcept;

	void BindPositive(InputCode inputCode) noexcept;
	void BindPositive(std::vector<InputCode> inputCodes) noexcept;
	void BindNegative(InputCode inputCode) noexcept;
	void BindNegative(std::vector<InputCode> inputCodes) noexcept;
	void BindAxis(InputCode inputCode) noexcept;
	void BindAxis(std::vector<InputCode> inputCodes) noexcept;
	void Bind(InputCode negative, InputCode positive) noexcept;
	void Bind(InputCode negative, InputCode positive, InputCode axis) noexcept;
	void Bind(std::vector<InputCode> negatives, std::vector<InputCode> positives) noexcept;
	void Bind(std::vector<InputCode> negatives, std::vector<InputCode> positives, std::vector<InputCode> axes) noexcept;

	void UnbindPositive(InputCode inputCode) noexcept;
	void UnbindNegative(InputCode inputCode) noexcept;
	void UnbindAxis(InputCode inputCode) noexcept;

	void ClearPositiveBinds() noexcept;
	void ClearNegativeBinds() noexcept;
	void ClearAxisBinds() noexcept;
	void ClearAllBinds() noexcept;

	const std::vector<InputCode>& GetPositiveBinds() const noexcept;
	const std::vector<InputCode>& GetNegativeBinds() const noexcept;
	const std::vector<InputCode>& GetAxisBinds() const noexcept;

private:
	std::vector<InputCode> positiveBinds;
	std::vector<InputCode> negativeBinds;
	std::vector<InputCode> axisBinds;
};