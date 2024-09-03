#pragma once

#include "GLIncludes.h"

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

	GamepadFirstButton	= GamepadA,
	GamepadLastButton		= GamepadDPLeft,

	GamepadLT		= 1014,
	GamepadRT		= 1015,
	GamepadLSX		= 1016,
	GamepadLSY		= 1017,
	GamepadRSX		= 1018,
	GamepadRSY		= 1019,

	GamepadFirstAxis	= GamepadLT,
	GamepadLastAxis	= GamepadRSY,

	GamepadUndefined = 1020,

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
	Gamepad0LT			= GamepadLT			+ (100 * 1),
	Gamepad0RT			= GamepadRT			+ (100 * 1),
	Gamepad0LSX			= GamepadLSX		+ (100 * 1),
	Gamepad0LSY			= GamepadLSY		+ (100 * 1),
	Gamepad0RSX			= GamepadRSX		+ (100 * 1),
	Gamepad0RSY			= GamepadRSY		+ (100 * 1),

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
	Gamepad1LT			= GamepadLT			+ (100 * 2),
	Gamepad1RT			= GamepadRT			+ (100 * 2),
	Gamepad1LSX			= GamepadLSX		+ (100 * 2),
	Gamepad1LSY			= GamepadLSY		+ (100 * 2),
	Gamepad1RSX			= GamepadRSX		+ (100 * 2),
	Gamepad1RSY			= GamepadRSY		+ (100 * 2),

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
	Gamepad2LT			= GamepadLT			+ (100 * 3),
	Gamepad2RT			= GamepadRT			+ (100 * 3),
	Gamepad2LSX			= GamepadLSX		+ (100 * 3),
	Gamepad2LSY			= GamepadLSY		+ (100 * 3),
	Gamepad2RSX			= GamepadRSX		+ (100 * 3),
	Gamepad2RSY			= GamepadRSY		+ (100 * 3),

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
	Gamepad3LT			= GamepadLT			+ (100 * 4),
	Gamepad3RT			= GamepadRT			+ (100 * 4),
	Gamepad3LSX			= GamepadLSX		+ (100 * 4),
	Gamepad3LSY			= GamepadLSY		+ (100 * 4),
	Gamepad3RSX			= GamepadRSX		+ (100 * 4),
	Gamepad3RSY			= GamepadRSY		+ (100 * 4),

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
	Gamepad4LT			= GamepadLT			+ (100 * 5),
	Gamepad4RT			= GamepadRT			+ (100 * 5),
	Gamepad4LSX			= GamepadLSX		+ (100 * 5),
	Gamepad4LSY			= GamepadLSY		+ (100 * 5),
	Gamepad4RSX			= GamepadRSX		+ (100 * 5),
	Gamepad4RSY			= GamepadRSY		+ (100 * 5),

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
	Gamepad5LT			= GamepadLT			+ (100 * 6),
	Gamepad5RT			= GamepadRT			+ (100 * 6),
	Gamepad5LSX			= GamepadLSX		+ (100 * 6),
	Gamepad5LSY			= GamepadLSY		+ (100 * 6),
	Gamepad5RSX			= GamepadRSX		+ (100 * 6),
	Gamepad5RSY			= GamepadRSY		+ (100 * 6),

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
	Gamepad6LT			= GamepadLT			+ (100 * 7),
	Gamepad6RT			= GamepadRT			+ (100 * 7),
	Gamepad6LSX			= GamepadLSX		+ (100 * 7),
	Gamepad6LSY			= GamepadLSY		+ (100 * 7),
	Gamepad6RSX			= GamepadRSX		+ (100 * 7),
	Gamepad6RSY			= GamepadRSY		+ (100 * 7),

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
	Gamepad7LT			= GamepadLT			+ (100 * 8),
	Gamepad7RT			= GamepadRT			+ (100 * 8),
	Gamepad7LSX			= GamepadLSX		+ (100 * 8),
	Gamepad7LSY			= GamepadLSY		+ (100 * 8),
	Gamepad7RSX			= GamepadRSX		+ (100 * 8),
	Gamepad7RSY			= GamepadRSY		+ (100 * 8),

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
	Gamepad8LT			= GamepadLT			+ (100 * 9),
	Gamepad8RT			= GamepadRT			+ (100 * 9),
	Gamepad8LSX			= GamepadLSX		+ (100 * 9),
	Gamepad8LSY			= GamepadLSY		+ (100 * 9),
	Gamepad8RSX			= GamepadRSX		+ (100 * 9),
	Gamepad8RSY			= GamepadRSY		+ (100 * 9),

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
	Gamepad9LT			= GamepadLT			+ (100 * 10),
	Gamepad9RT			= GamepadRT			+ (100 * 10),
	Gamepad9LSX			= GamepadLSX		+ (100 * 10),
	Gamepad9LSY			= GamepadLSY		+ (100 * 10),
	Gamepad9RSX			= GamepadRSX		+ (100 * 10),
	Gamepad9RSY			= GamepadRSY		+ (100 * 10),

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
	Gamepad10LT			= GamepadLT			+ (100 * 11),
	Gamepad10RT			= GamepadRT			+ (100 * 11),
	Gamepad10LSX		= GamepadLSX		+ (100 * 11),
	Gamepad10LSY		= GamepadLSY		+ (100 * 11),
	Gamepad10RSX		= GamepadRSX		+ (100 * 11),
	Gamepad10RSY		= GamepadRSY		+ (100 * 11),

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
	Gamepad11LT			= GamepadLT			+ (100 * 12),
	Gamepad11RT			= GamepadRT			+ (100 * 12),
	Gamepad11LSX		= GamepadLSX		+ (100 * 12),
	Gamepad11LSY		= GamepadLSY		+ (100 * 12),
	Gamepad11RSX		= GamepadRSX		+ (100 * 12),
	Gamepad11RSY		= GamepadRSY		+ (100 * 12),

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
	Gamepad12LT			= GamepadLT			+ (100 * 13),
	Gamepad12RT			= GamepadRT			+ (100 * 13),
	Gamepad12LSX		= GamepadLSX		+ (100 * 13),
	Gamepad12LSY		= GamepadLSY		+ (100 * 13),
	Gamepad12RSX		= GamepadRSX		+ (100 * 13),
	Gamepad12RSY		= GamepadRSY		+ (100 * 13),

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
	Gamepad13LT			= GamepadLT			+ (100 * 14),
	Gamepad13RT			= GamepadRT			+ (100 * 14),
	Gamepad13LSX		= GamepadLSX		+ (100 * 14),
	Gamepad13LSY		= GamepadLSY		+ (100 * 14),
	Gamepad13RSX		= GamepadRSX		+ (100 * 14),
	Gamepad13RSY		= GamepadRSY		+ (100 * 14),

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
	Gamepad14LT			= GamepadLT			+ (100 * 15),
	Gamepad14RT			= GamepadRT			+ (100 * 15),
	Gamepad14LSX		= GamepadLSX		+ (100 * 15),
	Gamepad14LSY		= GamepadLSY		+ (100 * 15),
	Gamepad14RSX		= GamepadRSX		+ (100 * 15),
	Gamepad14RSY		= GamepadRSY		+ (100 * 15),

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
	Gamepad15LT			= GamepadLT			+ (100 * 16),
	Gamepad15RT			= GamepadRT			+ (100 * 16),
	Gamepad15LSX		= GamepadLSX		+ (100 * 16),
	Gamepad15LSY		= GamepadLSY		+ (100 * 16),
	Gamepad15RSX		= GamepadRSX		+ (100 * 16),
	Gamepad15RSY		= GamepadRSY		+ (100 * 16),
};

const std::vector<InputCode> axisInputCodes = {
	InputCode::GamepadLT,
	InputCode::GamepadRT,
	InputCode::GamepadLSX,
	InputCode::GamepadLSY,
	InputCode::GamepadRSX,
	InputCode::GamepadRSY,

	InputCode::Gamepad0LT,
	InputCode::Gamepad0RT,
	InputCode::Gamepad0LSX,
	InputCode::Gamepad0LSY,
	InputCode::Gamepad0RSX,
	InputCode::Gamepad0RSY,

	InputCode::Gamepad1LT,
	InputCode::Gamepad1RT,
	InputCode::Gamepad1LSX,
	InputCode::Gamepad1LSY,
	InputCode::Gamepad1RSX,
	InputCode::Gamepad1RSY,

	InputCode::Gamepad2LT,
	InputCode::Gamepad2RT,
	InputCode::Gamepad2LSX,
	InputCode::Gamepad2LSY,
	InputCode::Gamepad2RSX,
	InputCode::Gamepad2RSY,

	InputCode::Gamepad3LT,
	InputCode::Gamepad3RT,
	InputCode::Gamepad3LSX,
	InputCode::Gamepad3LSY,
	InputCode::Gamepad3RSX,
	InputCode::Gamepad3RSY,

	InputCode::Gamepad4LT,
	InputCode::Gamepad4RT,
	InputCode::Gamepad4LSX,
	InputCode::Gamepad4LSY,
	InputCode::Gamepad4RSX,
	InputCode::Gamepad4RSY,

	InputCode::Gamepad5LT,
	InputCode::Gamepad5RT,
	InputCode::Gamepad5LSX,
	InputCode::Gamepad5LSY,
	InputCode::Gamepad5RSX,
	InputCode::Gamepad5RSY,

	InputCode::Gamepad6LT,
	InputCode::Gamepad6RT,
	InputCode::Gamepad6LSX,
	InputCode::Gamepad6LSY,
	InputCode::Gamepad6RSX,
	InputCode::Gamepad6RSY,

	InputCode::Gamepad7LT,
	InputCode::Gamepad7RT,
	InputCode::Gamepad7LSX,
	InputCode::Gamepad7LSY,
	InputCode::Gamepad7RSX,
	InputCode::Gamepad7RSY,

	InputCode::Gamepad8LT,
	InputCode::Gamepad8RT,
	InputCode::Gamepad8LSX,
	InputCode::Gamepad8LSY,
	InputCode::Gamepad8RSX,
	InputCode::Gamepad8RSY,

	InputCode::Gamepad9LT,
	InputCode::Gamepad9RT,
	InputCode::Gamepad9LSX,
	InputCode::Gamepad9LSY,
	InputCode::Gamepad9RSX,
	InputCode::Gamepad9RSY,

	InputCode::Gamepad10LT,
	InputCode::Gamepad10RT,
	InputCode::Gamepad10LSX,
	InputCode::Gamepad10LSY,
	InputCode::Gamepad10RSX,
	InputCode::Gamepad10RSY,

	InputCode::Gamepad11LT,
	InputCode::Gamepad11RT,
	InputCode::Gamepad11LSX,
	InputCode::Gamepad11LSY,
	InputCode::Gamepad11RSX,
	InputCode::Gamepad11RSY,

	InputCode::Gamepad12LT,
	InputCode::Gamepad12RT,
	InputCode::Gamepad12LSX,
	InputCode::Gamepad12LSY,
	InputCode::Gamepad12RSX,
	InputCode::Gamepad12RSY,

	InputCode::Gamepad13LT,
	InputCode::Gamepad13RT,
	InputCode::Gamepad13LSX,
	InputCode::Gamepad13LSY,
	InputCode::Gamepad13RSX,
	InputCode::Gamepad13RSY,

	InputCode::Gamepad14LT,
	InputCode::Gamepad14RT,
	InputCode::Gamepad14LSX,
	InputCode::Gamepad14LSY,
	InputCode::Gamepad14RSX,
	InputCode::Gamepad14RSY,

	InputCode::Gamepad15LT,
	InputCode::Gamepad15RT,
	InputCode::Gamepad15LSX,
	InputCode::Gamepad15LSY,
	InputCode::Gamepad15RSX,
	InputCode::Gamepad15RSY
};

inline constexpr bool IsAxis(InputCode inputCode) noexcept
{
	switch (inputCode)
	{
		case InputCode::GamepadLT:
		case InputCode::GamepadRT:
		case InputCode::GamepadLSX:
		case InputCode::GamepadLSY:
		case InputCode::GamepadRSX:
		case InputCode::GamepadRSY:
		case InputCode::Gamepad0LT:
		case InputCode::Gamepad0RT:
		case InputCode::Gamepad0LSX:
		case InputCode::Gamepad0LSY:
		case InputCode::Gamepad0RSX:
		case InputCode::Gamepad0RSY:
		case InputCode::Gamepad1LT:
		case InputCode::Gamepad1RT:
		case InputCode::Gamepad1LSX:
		case InputCode::Gamepad1LSY:
		case InputCode::Gamepad1RSX:
		case InputCode::Gamepad1RSY:
		case InputCode::Gamepad2LT:
		case InputCode::Gamepad2RT:
		case InputCode::Gamepad2LSX:
		case InputCode::Gamepad2LSY:
		case InputCode::Gamepad2RSX:
		case InputCode::Gamepad2RSY:
		case InputCode::Gamepad3LT:
		case InputCode::Gamepad3RT:
		case InputCode::Gamepad3LSX:
		case InputCode::Gamepad3LSY:
		case InputCode::Gamepad3RSX:
		case InputCode::Gamepad3RSY:
		case InputCode::Gamepad4LT:
		case InputCode::Gamepad4RT:
		case InputCode::Gamepad4LSX:
		case InputCode::Gamepad4LSY:
		case InputCode::Gamepad4RSX:
		case InputCode::Gamepad4RSY:
		case InputCode::Gamepad5LT:
		case InputCode::Gamepad5RT:
		case InputCode::Gamepad5LSX:
		case InputCode::Gamepad5LSY:
		case InputCode::Gamepad5RSX:
		case InputCode::Gamepad5RSY:
		case InputCode::Gamepad6LT:
		case InputCode::Gamepad6RT:
		case InputCode::Gamepad6LSX:
		case InputCode::Gamepad6LSY:
		case InputCode::Gamepad6RSX:
		case InputCode::Gamepad6RSY:
		case InputCode::Gamepad7LT:
		case InputCode::Gamepad7RT:
		case InputCode::Gamepad7LSX:
		case InputCode::Gamepad7LSY:
		case InputCode::Gamepad7RSX:
		case InputCode::Gamepad7RSY:
		case InputCode::Gamepad8LT:
		case InputCode::Gamepad8RT:
		case InputCode::Gamepad8LSX:
		case InputCode::Gamepad8LSY:
		case InputCode::Gamepad8RSX:
		case InputCode::Gamepad8RSY:
		case InputCode::Gamepad9LT:
		case InputCode::Gamepad9RT:
		case InputCode::Gamepad9LSX:
		case InputCode::Gamepad9LSY:
		case InputCode::Gamepad9RSX:
		case InputCode::Gamepad9RSY:
		case InputCode::Gamepad10LT:
		case InputCode::Gamepad10RT:
		case InputCode::Gamepad10LSX:
		case InputCode::Gamepad10LSY:
		case InputCode::Gamepad10RSX:
		case InputCode::Gamepad10RSY:
		case InputCode::Gamepad11LT:
		case InputCode::Gamepad11RT:
		case InputCode::Gamepad11LSX:
		case InputCode::Gamepad11LSY:
		case InputCode::Gamepad11RSX:
		case InputCode::Gamepad11RSY:
		case InputCode::Gamepad12LT:
		case InputCode::Gamepad12RT:
		case InputCode::Gamepad12LSX:
		case InputCode::Gamepad12LSY:
		case InputCode::Gamepad12RSX:
		case InputCode::Gamepad12RSY:
		case InputCode::Gamepad13LT:
		case InputCode::Gamepad13RT:
		case InputCode::Gamepad13LSX:
		case InputCode::Gamepad13LSY:
		case InputCode::Gamepad13RSX:
		case InputCode::Gamepad13RSY:
		case InputCode::Gamepad14LT:
		case InputCode::Gamepad14RT:
		case InputCode::Gamepad14LSX:
		case InputCode::Gamepad14LSY:
		case InputCode::Gamepad14RSX:
		case InputCode::Gamepad14RSY:
		case InputCode::Gamepad15LT:
		case InputCode::Gamepad15RT:
		case InputCode::Gamepad15LSX:
		case InputCode::Gamepad15LSY:
		case InputCode::Gamepad15RSX:
		case InputCode::Gamepad15RSY:
		return true;
	default:
		return false;
	}
}
