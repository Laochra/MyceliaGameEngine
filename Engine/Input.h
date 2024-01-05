#pragma once

#include "GLIncludes.h"
#include <set>
#include <vector>

enum KeyCode
{
	// Mouse
	MouseLeft		= GLFW_MOUSE_BUTTON_LEFT,
	MouseRight		= GLFW_MOUSE_BUTTON_RIGHT,
	MouseMiddle		= GLFW_MOUSE_BUTTON_MIDDLE,
	Mouse3			= 3,
	Mouse4			= 4,
	Mouse5			= 5,
	Mouse6			= 6,
	Mouse7			= 7,

	// Special
	Esc				= GLFW_KEY_ESCAPE,
	Tab				= GLFW_KEY_TAB,
	Enter			= GLFW_KEY_ENTER,
	keypadEnter		= GLFW_KEY_KP_ENTER,
	Backspace		= GLFW_KEY_BACKSPACE,
	Delete			= GLFW_KEY_DELETE,
	Insert			= GLFW_KEY_INSERT,
	Pause			= GLFW_KEY_PAUSE,
	Print			= GLFW_KEY_PRINT_SCREEN,
	Menu			= GLFW_KEY_MENU,

	// Navigation
	Up				= GLFW_KEY_UP,
	Down			= GLFW_KEY_DOWN,
	Left			= GLFW_KEY_LEFT,
	Right			= GLFW_KEY_RIGHT,
	PageUp			= GLFW_KEY_PAGE_UP,
	PageDown		= GLFW_KEY_PAGE_DOWN,
	Home			= GLFW_KEY_HOME,
	End				= GLFW_KEY_END,

	// Punctuation
	Space			= GLFW_KEY_SPACE,
	Comma			= GLFW_KEY_COMMA,
	Period			= GLFW_KEY_PERIOD,
	Semicolon		= GLFW_KEY_SEMICOLON,
	Hyphen			= GLFW_KEY_MINUS,
	Apostrophe		= GLFW_KEY_APOSTROPHE,
	Grave			= GLFW_KEY_GRAVE_ACCENT,
	ForwardSlash	= GLFW_KEY_SLASH,
	BackSlash		= GLFW_KEY_BACKSLASH,
	LBracket		= GLFW_KEY_LEFT_BRACKET,
	RBracket		= GLFW_KEY_RIGHT_BRACKET,

	// Maths
	Decimal			= GLFW_KEY_KP_DECIMAL,
	Add				= GLFW_KEY_KP_ADD,
	Subtract		= GLFW_KEY_KP_SUBTRACT,
	Multiply		= GLFW_KEY_KP_MULTIPLY,
	Divide			= GLFW_KEY_KP_DIVIDE,
	Equals			= GLFW_KEY_EQUAL,
	keypadEquals	= GLFW_KEY_KP_EQUAL,

	// Locks
	CapsLock		= GLFW_KEY_CAPS_LOCK,
	NumLock			= GLFW_KEY_NUM_LOCK,
	ScrollLock		= GLFW_KEY_SCROLL_LOCK,

	// Modifiers
	LShift			= GLFW_KEY_LEFT_SHIFT,
	RShift			= GLFW_KEY_RIGHT_SHIFT,
	LControl		= GLFW_KEY_LEFT_CONTROL,
	RControl		= GLFW_KEY_RIGHT_CONTROL,
	LAlt			= GLFW_KEY_LEFT_ALT,
	RAlt			= GLFW_KEY_RIGHT_ALT,
	LSuper			= GLFW_KEY_LEFT_SUPER,
	RSuper			= GLFW_KEY_RIGHT_SUPER,

	// Function
	f1				= GLFW_KEY_F1,
	f2				= GLFW_KEY_F2,
	f3				= GLFW_KEY_F3,
	f4				= GLFW_KEY_F4,
	f5				= GLFW_KEY_F5,
	f6				= GLFW_KEY_F6,
	f7				= GLFW_KEY_F7,
	f8				= GLFW_KEY_F8,
	f9				= GLFW_KEY_F9,
	f10				= GLFW_KEY_F10,
	f11				= GLFW_KEY_F11,
	f12				= GLFW_KEY_F12,
	f13				= GLFW_KEY_F13,
	f14				= GLFW_KEY_F14,
	f15				= GLFW_KEY_F15,
	f16				= GLFW_KEY_F16,
	f17				= GLFW_KEY_F17,
	f18				= GLFW_KEY_F18,
	f19				= GLFW_KEY_F19,
	f20				= GLFW_KEY_F20,
	f21				= GLFW_KEY_F21,
	f22				= GLFW_KEY_F22,
	f23				= GLFW_KEY_F23,
	f24				= GLFW_KEY_F24,
	f25				= GLFW_KEY_F25,

	// Numbers
	num0			= GLFW_KEY_0,
	num1			= GLFW_KEY_1,
	num2			= GLFW_KEY_2,
	num3			= GLFW_KEY_3,
	num4			= GLFW_KEY_4,
	num5			= GLFW_KEY_5,
	num6			= GLFW_KEY_6,
	num7			= GLFW_KEY_7,
	num8			= GLFW_KEY_8,
	num9			= GLFW_KEY_9,
	keypad0			= GLFW_KEY_KP_0,
	keypad1			= GLFW_KEY_KP_1,
	keypad2			= GLFW_KEY_KP_2,
	keypad3			= GLFW_KEY_KP_3,
	keypad4			= GLFW_KEY_KP_4,
	keypad5			= GLFW_KEY_KP_5,
	keypad6			= GLFW_KEY_KP_6,
	keypad7			= GLFW_KEY_KP_7,
	keypad8			= GLFW_KEY_KP_8,
	keypad9			= GLFW_KEY_KP_9,

	// Letters
	A				= GLFW_KEY_A,
	B				= GLFW_KEY_B,
	C				= GLFW_KEY_C,
	D				= GLFW_KEY_D,
	E				= GLFW_KEY_E,
	F				= GLFW_KEY_F,
	G				= GLFW_KEY_G,
	H				= GLFW_KEY_H,
	I				= GLFW_KEY_I,
	J				= GLFW_KEY_J,
	K				= GLFW_KEY_K,
	L				= GLFW_KEY_L,
	M				= GLFW_KEY_M,
	N				= GLFW_KEY_N,
	O				= GLFW_KEY_O,
	P				= GLFW_KEY_P,
	Q				= GLFW_KEY_Q,
	R				= GLFW_KEY_R,
	S				= GLFW_KEY_S,
	T				= GLFW_KEY_T,
	U				= GLFW_KEY_U,
	V				= GLFW_KEY_V,
	W				= GLFW_KEY_W,
	X				= GLFW_KEY_X,
	Y				= GLFW_KEY_Y,
	Z				= GLFW_KEY_Z,
};

class Input
{
public:
	bool enabled = true;

	std::set<KeyCode>* downKeys;
	std::set<KeyCode>* pressedKeys;
	std::set<KeyCode>* releasedKeys;

	vec2 cursorPos = vec2(0, 0);
	vec2 cursorMovement = vec2(0, 0);
	vec2 scrollInput = vec2(0, 0);

	Input();
	~Input();
	Input(const Input& other) = delete;
	Input& operator=(const Input& other) = delete;

	bool GetKeyDown(KeyCode keyCode) const noexcept;
	bool GetKeyPressed(KeyCode keyCode) const noexcept;
	bool GetKeyReleased(KeyCode keyCode) const noexcept;

	void Update();
};

extern Input* input;

extern const int keyAmount;
extern const char* keys[];

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void ScrollCallback(GLFWwindow* window, double x, double y);
void CursorPosCallback(GLFWwindow* window, double x, double y);

class Keybind
{
public:
	Keybind() noexcept;
	Keybind(KeyCode keyCode) noexcept;
	~Keybind() noexcept;
	Keybind(const Keybind& other) = delete;
	Keybind& operator=(const Keybind& other) = delete;

	bool down() const noexcept;
	bool pressed() const noexcept;
	bool released() const noexcept;
	operator bool() const { return down(); }

	void Bind(KeyCode keyCode) noexcept;
	void Bind(std::vector<KeyCode> keyCodes) noexcept;
	void Unbind(KeyCode keyCode) noexcept;
	void ClearBinds() noexcept;

	std::vector<KeyCode> GetBinds() { return *keys; }

private:
	std::vector<KeyCode>* keys;
};

class KeyAxis
{
public:
	KeyAxis() noexcept;
	KeyAxis(KeyCode negative, KeyCode positive) noexcept;
	~KeyAxis() noexcept;
	KeyAxis(const KeyAxis& other) = delete;
	KeyAxis& operator=(const KeyAxis& other) = delete;

	float value() const noexcept;
	operator float() const { return value(); }

	void BindPositive(KeyCode keyCode) noexcept;
	void BindPositive(std::vector<KeyCode> keyCodes) noexcept;
	void BindNegative(KeyCode keyCode) noexcept;
	void BindNegative(std::vector<KeyCode> keyCodes) noexcept;
	void BindPair(KeyCode negative, KeyCode positive) noexcept;

	void UnbindPositive(KeyCode keyCode) noexcept;
	void UnbindNegative(KeyCode keyCode) noexcept;

	void ClearPositiveBinds() noexcept;
	void ClearNegativeBinds() noexcept;
	void ClearAllBinds() noexcept;

	std::vector<KeyCode> GetPositiveBinds() { return *positiveKeys; }
	std::vector<KeyCode> GetNegativeBinds() { return *negativeKeys; }

private:
	std::vector<KeyCode>* positiveKeys;
	std::vector<KeyCode>* negativeKeys;
};