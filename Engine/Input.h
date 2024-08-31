#pragma once

#include "GLIncludes.h"
#include <set>
#include <vector>

enum class InputCode
{
	// Mouse
	MouseLeft = GLFW_MOUSE_BUTTON_LEFT,
	MouseRight = GLFW_MOUSE_BUTTON_RIGHT,
	MouseMiddle = GLFW_MOUSE_BUTTON_MIDDLE,
	Mouse3 = 3,
	Mouse4 = 4,
	Mouse5 = 5,
	Mouse6 = 6,
	Mouse7 = 7,

	// Special
	Esc = GLFW_KEY_ESCAPE,
	Tab = GLFW_KEY_TAB,
	Enter = GLFW_KEY_ENTER,
	keypadEnter = GLFW_KEY_KP_ENTER,
	Backspace = GLFW_KEY_BACKSPACE,
	Delete = GLFW_KEY_DELETE,
	Insert = GLFW_KEY_INSERT,
	Pause = GLFW_KEY_PAUSE,
	Print = GLFW_KEY_PRINT_SCREEN,
	Menu = GLFW_KEY_MENU,

	// Navigation
	Up = GLFW_KEY_UP,
	Down = GLFW_KEY_DOWN,
	Left = GLFW_KEY_LEFT,
	Right = GLFW_KEY_RIGHT,
	PageUp = GLFW_KEY_PAGE_UP,
	PageDown = GLFW_KEY_PAGE_DOWN,
	Home = GLFW_KEY_HOME,
	End = GLFW_KEY_END,

	// Punctuation
	Space = GLFW_KEY_SPACE,
	Comma = GLFW_KEY_COMMA,
	Period = GLFW_KEY_PERIOD,
	Semicolon = GLFW_KEY_SEMICOLON,
	Hyphen = GLFW_KEY_MINUS,
	Apostrophe = GLFW_KEY_APOSTROPHE,
	Grave = GLFW_KEY_GRAVE_ACCENT,
	ForwardSlash = GLFW_KEY_SLASH,
	BackSlash = GLFW_KEY_BACKSLASH,
	LBracket = GLFW_KEY_LEFT_BRACKET,
	RBracket = GLFW_KEY_RIGHT_BRACKET,

	// Maths
	Decimal = GLFW_KEY_KP_DECIMAL,
	Add = GLFW_KEY_KP_ADD,
	Subtract = GLFW_KEY_KP_SUBTRACT,
	Multiply = GLFW_KEY_KP_MULTIPLY,
	Divide = GLFW_KEY_KP_DIVIDE,
	Equals = GLFW_KEY_EQUAL,
	keypadEquals = GLFW_KEY_KP_EQUAL,

	// Locks
	CapsLock = GLFW_KEY_CAPS_LOCK,
	NumLock = GLFW_KEY_NUM_LOCK,
	ScrollLock = GLFW_KEY_SCROLL_LOCK,

	// Modifiers
	LShift = GLFW_KEY_LEFT_SHIFT,
	RShift = GLFW_KEY_RIGHT_SHIFT,
	LControl = GLFW_KEY_LEFT_CONTROL,
	RControl = GLFW_KEY_RIGHT_CONTROL,
	LAlt = GLFW_KEY_LEFT_ALT,
	RAlt = GLFW_KEY_RIGHT_ALT,
	LSuper = GLFW_KEY_LEFT_SUPER,
	RSuper = GLFW_KEY_RIGHT_SUPER,

	// Function
	f1 = GLFW_KEY_F1,
	f2 = GLFW_KEY_F2,
	f3 = GLFW_KEY_F3,
	f4 = GLFW_KEY_F4,
	f5 = GLFW_KEY_F5,
	f6 = GLFW_KEY_F6,
	f7 = GLFW_KEY_F7,
	f8 = GLFW_KEY_F8,
	f9 = GLFW_KEY_F9,
	f10 = GLFW_KEY_F10,
	f11 = GLFW_KEY_F11,
	f12 = GLFW_KEY_F12,
	f13 = GLFW_KEY_F13,
	f14 = GLFW_KEY_F14,
	f15 = GLFW_KEY_F15,
	f16 = GLFW_KEY_F16,
	f17 = GLFW_KEY_F17,
	f18 = GLFW_KEY_F18,
	f19 = GLFW_KEY_F19,
	f20 = GLFW_KEY_F20,
	f21 = GLFW_KEY_F21,
	f22 = GLFW_KEY_F22,
	f23 = GLFW_KEY_F23,
	f24 = GLFW_KEY_F24,
	f25 = GLFW_KEY_F25,

	// Numbers
	num0 = GLFW_KEY_0,
	num1 = GLFW_KEY_1,
	num2 = GLFW_KEY_2,
	num3 = GLFW_KEY_3,
	num4 = GLFW_KEY_4,
	num5 = GLFW_KEY_5,
	num6 = GLFW_KEY_6,
	num7 = GLFW_KEY_7,
	num8 = GLFW_KEY_8,
	num9 = GLFW_KEY_9,
	keypad0 = GLFW_KEY_KP_0,
	keypad1 = GLFW_KEY_KP_1,
	keypad2 = GLFW_KEY_KP_2,
	keypad3 = GLFW_KEY_KP_3,
	keypad4 = GLFW_KEY_KP_4,
	keypad5 = GLFW_KEY_KP_5,
	keypad6 = GLFW_KEY_KP_6,
	keypad7 = GLFW_KEY_KP_7,
	keypad8 = GLFW_KEY_KP_8,
	keypad9 = GLFW_KEY_KP_9,

	// Letters
	A = GLFW_KEY_A,
	B = GLFW_KEY_B,
	C = GLFW_KEY_C,
	D = GLFW_KEY_D,
	E = GLFW_KEY_E,
	F = GLFW_KEY_F,
	G = GLFW_KEY_G,
	H = GLFW_KEY_H,
	I = GLFW_KEY_I,
	J = GLFW_KEY_J,
	K = GLFW_KEY_K,
	L = GLFW_KEY_L,
	M = GLFW_KEY_M,
	N = GLFW_KEY_N,
	O = GLFW_KEY_O,
	P = GLFW_KEY_P,
	Q = GLFW_KEY_Q,
	R = GLFW_KEY_R,
	S = GLFW_KEY_S,
	T = GLFW_KEY_T,
	U = GLFW_KEY_U,
	V = GLFW_KEY_V,
	W = GLFW_KEY_W,
	X = GLFW_KEY_X,
	Y = GLFW_KEY_Y,
	Z = GLFW_KEY_Z,

	// Any Gamepad Input
	GamepadA			= 1000,
	GamepadB			= 1001,
	GamepadX			= 1002,
	GamepadY			= 1003,
	GamepadLB		= 1004,
	GamepadRB		= 1005,
	GamepadLM		= 1006,
	GamepadRM		= 1007,
	GamepadLS		= 1008,
	GamepadRS		= 1009,
	GamepadDPUp		= 1010,
	GamepadDPRight	= 1011,
	GamepadDPDown	= 1012,
	GamepadDPLeft	= 1013,

	GamepadFirst	= GamepadA,
	GamepadLast		= GamepadDPLeft,

	// Specific Gamepad Input
	Gamepad0A			= GamepadA			+ (100 * 1),
	Gamepad0B			= GamepadB			+ (100 * 1),
	Gamepad0X			= GamepadX			+ (100 * 1),
	Gamepad0Y			= GamepadY			+ (100 * 1),
	Gamepad0LB			= GamepadLB			+ (100 * 1),
	Gamepad0RB			= GamepadRB			+ (100 * 1),
	Gamepad0LM			= GamepadLM			+ (100 * 1),
	Gamepad0RM			= GamepadRM			+ (100 * 1),
	Gamepad0LS			= GamepadLS			+ (100 * 1),
	Gamepad0RS			= GamepadRS			+ (100 * 1),
	Gamepad0DPUp		= GamepadDPUp		+ (100 * 1),
	Gamepad0DPRight	= GamepadDPRight	+ (100 * 1),
	Gamepad0DPDown		= GamepadDPDown	+ (100 * 1),
	Gamepad0DPLeft		= GamepadDPLeft	+ (100 * 1),

	Gamepad1A			= GamepadA			+ (100 * 2),
	Gamepad1B			= GamepadB			+ (100 * 2),
	Gamepad1X			= GamepadX			+ (100 * 2),
	Gamepad1Y			= GamepadY			+ (100 * 2),
	Gamepad1LB			= GamepadLB			+ (100 * 2),
	Gamepad1RB			= GamepadRB			+ (100 * 2),
	Gamepad1LM			= GamepadLM			+ (100 * 2),
	Gamepad1RM			= GamepadRM			+ (100 * 2),
	Gamepad1LS			= GamepadLS			+ (100 * 2),
	Gamepad1RS			= GamepadRS			+ (100 * 2),
	Gamepad1DPUp		= GamepadDPUp		+ (100 * 2),
	Gamepad1DPRight	= GamepadDPRight	+ (100 * 2),
	Gamepad1DPDown		= GamepadDPDown	+ (100 * 2),
	Gamepad1DPLeft		= GamepadDPLeft	+ (100 * 2),

	Gamepad2A			= GamepadA			+ (100 * 3),
	Gamepad2B			= GamepadB			+ (100 * 3),
	Gamepad2X			= GamepadX			+ (100 * 3),
	Gamepad2Y			= GamepadY			+ (100 * 3),
	Gamepad2LB			= GamepadLB			+ (100 * 3),
	Gamepad2RB			= GamepadRB			+ (100 * 3),
	Gamepad2LM			= GamepadLM			+ (100 * 3),
	Gamepad2RM			= GamepadRM			+ (100 * 3),
	Gamepad2LS			= GamepadLS			+ (100 * 3),
	Gamepad2RS			= GamepadRS			+ (100 * 3),
	Gamepad2DPUp		= GamepadDPUp		+ (100 * 3),
	Gamepad2DPRight	= GamepadDPRight	+ (100 * 3),
	Gamepad2DPDown		= GamepadDPDown	+ (100 * 3),
	Gamepad2DPLeft		= GamepadDPLeft	+ (100 * 3),

	Gamepad3A			= GamepadA			+ (100 * 4),
	Gamepad3B			= GamepadB			+ (100 * 4),
	Gamepad3X			= GamepadX			+ (100 * 4),
	Gamepad3Y			= GamepadY			+ (100 * 4),
	Gamepad3LB			= GamepadLB			+ (100 * 4),
	Gamepad3RB			= GamepadRB			+ (100 * 4),
	Gamepad3LM			= GamepadLM			+ (100 * 4),
	Gamepad3RM			= GamepadRM			+ (100 * 4),
	Gamepad3LS			= GamepadLS			+ (100 * 4),
	Gamepad3RS			= GamepadRS			+ (100 * 4),
	Gamepad3DPUp		= GamepadDPUp		+ (100 * 4),
	Gamepad3DPRight	= GamepadDPRight	+ (100 * 4),
	Gamepad3DPDown		= GamepadDPDown	+ (100 * 4),
	Gamepad3DPLeft		= GamepadDPLeft	+ (100 * 4),

	Gamepad4A			= GamepadA			+ (100 * 5),
	Gamepad4B			= GamepadB			+ (100 * 5),
	Gamepad4X			= GamepadX			+ (100 * 5),
	Gamepad4Y			= GamepadY			+ (100 * 5),
	Gamepad4LB			= GamepadLB			+ (100 * 5),
	Gamepad4RB			= GamepadRB			+ (100 * 5),
	Gamepad4LM			= GamepadLM			+ (100 * 5),
	Gamepad4RM			= GamepadRM			+ (100 * 5),
	Gamepad4LS			= GamepadLS			+ (100 * 5),
	Gamepad4RS			= GamepadRS			+ (100 * 5),
	Gamepad4DPUp		= GamepadDPUp		+ (100 * 5),
	Gamepad4DPRight	= GamepadDPRight	+ (100 * 5),
	Gamepad4DPDown		= GamepadDPDown	+ (100 * 5),
	Gamepad4DPLeft		= GamepadDPLeft	+ (100 * 5),

	Gamepad5A			= GamepadA			+ (100 * 6),
	Gamepad5B			= GamepadB			+ (100 * 6),
	Gamepad5X			= GamepadX			+ (100 * 6),
	Gamepad5Y			= GamepadY			+ (100 * 6),
	Gamepad5LB			= GamepadLB			+ (100 * 6),
	Gamepad5RB			= GamepadRB			+ (100 * 6),
	Gamepad5LM			= GamepadLM			+ (100 * 6),
	Gamepad5RM			= GamepadRM			+ (100 * 6),
	Gamepad5LS			= GamepadLS			+ (100 * 6),
	Gamepad5RS			= GamepadRS			+ (100 * 6),
	Gamepad5DPUp		= GamepadDPUp		+ (100 * 6),
	Gamepad5DPRight	= GamepadDPRight	+ (100 * 6),
	Gamepad5DPDown		= GamepadDPDown	+ (100 * 6),
	Gamepad5DPLeft		= GamepadDPLeft	+ (100 * 6),

	Gamepad6A			= GamepadA			+ (100 * 7),
	Gamepad6B			= GamepadB			+ (100 * 7),
	Gamepad6X			= GamepadX			+ (100 * 7),
	Gamepad6Y			= GamepadY			+ (100 * 7),
	Gamepad6LB			= GamepadLB			+ (100 * 7),
	Gamepad6RB			= GamepadRB			+ (100 * 7),
	Gamepad6LM			= GamepadLM			+ (100 * 7),
	Gamepad6RM			= GamepadRM			+ (100 * 7),
	Gamepad6LS			= GamepadLS			+ (100 * 7),
	Gamepad6RS			= GamepadRS			+ (100 * 7),
	Gamepad6DPUp		= GamepadDPUp		+ (100 * 7),
	Gamepad6DPRight	= GamepadDPRight	+ (100 * 7),
	Gamepad6DPDown		= GamepadDPDown	+ (100 * 7),
	Gamepad6DPLeft		= GamepadDPLeft	+ (100 * 7),

	Gamepad7A			= GamepadA			+ (100 * 8),
	Gamepad7B			= GamepadB			+ (100 * 8),
	Gamepad7X			= GamepadX			+ (100 * 8),
	Gamepad7Y			= GamepadY			+ (100 * 8),
	Gamepad7LB			= GamepadLB			+ (100 * 8),
	Gamepad7RB			= GamepadRB			+ (100 * 8),
	Gamepad7LM			= GamepadLM			+ (100 * 8),
	Gamepad7RM			= GamepadRM			+ (100 * 8),
	Gamepad7LS			= GamepadLS			+ (100 * 8),
	Gamepad7RS			= GamepadRS			+ (100 * 8),
	Gamepad7DPUp		= GamepadDPUp		+ (100 * 8),
	Gamepad7DPRight	= GamepadDPRight	+ (100 * 8),
	Gamepad7DPDown		= GamepadDPDown	+ (100 * 8),
	Gamepad7DPLeft		= GamepadDPLeft	+ (100 * 8),

	Gamepad8A			= GamepadA			+ (100 * 9),
	Gamepad8B			= GamepadB			+ (100 * 9),
	Gamepad8X			= GamepadX			+ (100 * 9),
	Gamepad8Y			= GamepadY			+ (100 * 9),
	Gamepad8LB			= GamepadLB			+ (100 * 9),
	Gamepad8RB			= GamepadRB			+ (100 * 9),
	Gamepad8LM			= GamepadLM			+ (100 * 9),
	Gamepad8RM			= GamepadRM			+ (100 * 9),
	Gamepad8LS			= GamepadLS			+ (100 * 9),
	Gamepad8RS			= GamepadRS			+ (100 * 9),
	Gamepad8DPUp		= GamepadDPUp		+ (100 * 9),
	Gamepad8DPRight	= GamepadDPRight	+ (100 * 9),
	Gamepad8DPDown		= GamepadDPDown	+ (100 * 9),
	Gamepad8DPLeft		= GamepadDPLeft	+ (100 * 9),

	Gamepad9A			= GamepadA			+ (100 * 10),
	Gamepad9B			= GamepadB			+ (100 * 10),
	Gamepad9X			= GamepadX			+ (100 * 10),
	Gamepad9Y			= GamepadY			+ (100 * 10),
	Gamepad9LB			= GamepadLB			+ (100 * 10),
	Gamepad9RB			= GamepadRB			+ (100 * 10),
	Gamepad9LM			= GamepadLM			+ (100 * 10),
	Gamepad9RM			= GamepadRM			+ (100 * 10),
	Gamepad9LS			= GamepadLS			+ (100 * 10),
	Gamepad9RS			= GamepadRS			+ (100 * 10),
	Gamepad9DPUp		= GamepadDPUp		+ (100 * 10),
	Gamepad9DPRight	= GamepadDPRight	+ (100 * 10),
	Gamepad9DPDown		= GamepadDPDown	+ (100 * 10),
	Gamepad9DPLeft		= GamepadDPLeft	+ (100 * 10),

	Gamepad10A			= GamepadA			+ (100 * 11),
	Gamepad10B			= GamepadB			+ (100 * 11),
	Gamepad10X			= GamepadX			+ (100 * 11),
	Gamepad10Y			= GamepadY			+ (100 * 11),
	Gamepad10LB			= GamepadLB			+ (100 * 11),
	Gamepad10RB			= GamepadRB			+ (100 * 11),
	Gamepad10LM			= GamepadLM			+ (100 * 11),
	Gamepad10RM			= GamepadRM			+ (100 * 11),
	Gamepad10LS			= GamepadLS			+ (100 * 11),
	Gamepad10RS			= GamepadRS			+ (100 * 11),
	Gamepad10DPUp		= GamepadDPUp		+ (100 * 11),
	Gamepad10DPRight	= GamepadDPRight	+ (100 * 11),
	Gamepad10DPDown	= GamepadDPDown	+ (100 * 11),
	Gamepad10DPLeft	= GamepadDPLeft	+ (100 * 11),

	Gamepad11A			= GamepadA			+ (100 * 12),
	Gamepad11B			= GamepadB			+ (100 * 12),
	Gamepad11X			= GamepadX			+ (100 * 12),
	Gamepad11Y			= GamepadY			+ (100 * 12),
	Gamepad11LB			= GamepadLB			+ (100 * 12),
	Gamepad11RB			= GamepadRB			+ (100 * 12),
	Gamepad11LM			= GamepadLM			+ (100 * 12),
	Gamepad11RM			= GamepadRM			+ (100 * 12),
	Gamepad11LS			= GamepadLS			+ (100 * 12),
	Gamepad11RS			= GamepadRS			+ (100 * 12),
	Gamepad11DPUp		= GamepadDPUp		+ (100 * 12),
	Gamepad11DPRight	= GamepadDPRight	+ (100 * 12),
	Gamepad11DPDown	= GamepadDPDown	+ (100 * 12),
	Gamepad11DPLeft	= GamepadDPLeft	+ (100 * 12),

	Gamepad12A			= GamepadA			+ (100 * 13),
	Gamepad12B			= GamepadB			+ (100 * 13),
	Gamepad12X			= GamepadX			+ (100 * 13),
	Gamepad12Y			= GamepadY			+ (100 * 13),
	Gamepad12LB			= GamepadLB			+ (100 * 13),
	Gamepad12RB			= GamepadRB			+ (100 * 13),
	Gamepad12LM			= GamepadLM			+ (100 * 13),
	Gamepad12RM			= GamepadRM			+ (100 * 13),
	Gamepad12LS			= GamepadLS			+ (100 * 13),
	Gamepad12RS			= GamepadRS			+ (100 * 13),
	Gamepad12DPUp		= GamepadDPUp		+ (100 * 13),
	Gamepad12DPRight	= GamepadDPRight	+ (100 * 13),
	Gamepad12DPDown	= GamepadDPDown	+ (100 * 13),
	Gamepad12DPLeft	= GamepadDPLeft	+ (100 * 13),

	Gamepad13A			= GamepadA			+ (100 * 14),
	Gamepad13B			= GamepadB			+ (100 * 14),
	Gamepad13X			= GamepadX			+ (100 * 14),
	Gamepad13Y			= GamepadY			+ (100 * 14),
	Gamepad13LB			= GamepadLB			+ (100 * 14),
	Gamepad13RB			= GamepadRB			+ (100 * 14),
	Gamepad13LM			= GamepadLM			+ (100 * 14),
	Gamepad13RM			= GamepadRM			+ (100 * 14),
	Gamepad13LS			= GamepadLS			+ (100 * 14),
	Gamepad13RS			= GamepadRS			+ (100 * 14),
	Gamepad13DPUp		= GamepadDPUp		+ (100 * 14),
	Gamepad13DPRight	= GamepadDPRight	+ (100 * 14),
	Gamepad13DPDown	= GamepadDPDown	+ (100 * 14),
	Gamepad13DPLeft	= GamepadDPLeft	+ (100 * 14),

	Gamepad14A			= GamepadA			+ (100 * 15),
	Gamepad14B			= GamepadB			+ (100 * 15),
	Gamepad14X			= GamepadX			+ (100 * 15),
	Gamepad14Y			= GamepadY			+ (100 * 15),
	Gamepad14LB			= GamepadLB			+ (100 * 15),
	Gamepad14RB			= GamepadRB			+ (100 * 15),
	Gamepad14LM			= GamepadLM			+ (100 * 15),
	Gamepad14RM			= GamepadRM			+ (100 * 15),
	Gamepad14LS			= GamepadLS			+ (100 * 15),
	Gamepad14RS			= GamepadRS			+ (100 * 15),
	Gamepad14DPUp		= GamepadDPUp		+ (100 * 15),
	Gamepad14DPRight	= GamepadDPRight	+ (100 * 15),
	Gamepad14DPDown	= GamepadDPDown	+ (100 * 15),
	Gamepad14DPLeft	= GamepadDPLeft	+ (100 * 15),

	Gamepad15A			= GamepadA			+ (100 * 16),
	Gamepad15B			= GamepadB			+ (100 * 16),
	Gamepad15X			= GamepadX			+ (100 * 16),
	Gamepad15Y			= GamepadY			+ (100 * 16),
	Gamepad15LB			= GamepadLB			+ (100 * 16),
	Gamepad15RB			= GamepadRB			+ (100 * 16),
	Gamepad15LM			= GamepadLM			+ (100 * 16),
	Gamepad15RM			= GamepadRM			+ (100 * 16),
	Gamepad15LS			= GamepadLS			+ (100 * 16),
	Gamepad15RS			= GamepadRS			+ (100 * 16),
	Gamepad15DPUp		= GamepadDPUp		+ (100 * 16),
	Gamepad15DPRight	= GamepadDPRight	+ (100 * 16),
	Gamepad15DPDown	= GamepadDPDown	+ (100 * 16),
	Gamepad15DPLeft	= GamepadDPLeft	+ (100 * 16),
};

class Input
{
public:
	bool enabled = true;

	std::vector<bool> presentGamepads;

	std::set<InputCode>* downInputs;
	std::set<InputCode>* pressedInputs;
	std::set<InputCode>* releasedInputs;

	vec2 cursorPos = vec2(0, 0);
	vec2 cursorMovement = vec2(0, 0);
	vec2 scrollInput = vec2(0, 0);

	Input();
	~Input();
	Input(const Input& other) = delete;
	Input& operator=(const Input& other) = delete;

	bool GetInputDown(InputCode inputCode) const noexcept;
	bool GetInputPressed(InputCode inputCode) const noexcept;
	bool GetInputReleased(InputCode inputCode) const noexcept;

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