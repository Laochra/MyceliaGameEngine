#include "Input.h"

#include "GeneralMacros.h"

#include "AppInfo.h"

#include "Debug.h"

#include <array>
#include <map>

const std::map<InputCode, const char*> inputCodeToName = {
		// Mouse
		{ InputCode::MouseLeft, "Mouse Left" },
		{ InputCode::MouseRight, "Mouse Right" },
		{ InputCode::MouseMiddle, "Mouse Middle" },
		{ InputCode::Mouse3, "Mouse 3" },
		{ InputCode::Mouse4, "Mouse 4" },
		{ InputCode::Mouse5, "Mouse 5" },
		{ InputCode::Mouse6, "Mouse 6" },
		{ InputCode::Mouse7, "Mouse 7" },

		// Special
		{ InputCode::Esc, "Esc" },
		{ InputCode::Tab, "Tab" },
		{ InputCode::Enter, "Enter" },
		{ InputCode::keypadEnter, "Enter (Keypad)" },
		{ InputCode::Backspace, "Backspace" },
		{ InputCode::Delete, "Delete" },
		{ InputCode::Insert, "Insert" },
		{ InputCode::Pause, "Pause" },
		{ InputCode::Print, "Print" },
		{ InputCode::Menu, "Menu" },

		// Navigation
		{ InputCode::Up, "Up" },
		{ InputCode::Down, "Down" },
		{ InputCode::Left, "Left" },
		{ InputCode::Right, "Right" },
		{ InputCode::PageUp, "Page Up" },
		{ InputCode::PageDown, "Page Down" },
		{ InputCode::Home, "Home" },
		{ InputCode::End, "End" },

		// Punctuation
		{ InputCode::Space, "Space" },
		{ InputCode::Comma, "," },
		{ InputCode::Period, "." },
		{ InputCode::Semicolon, ";" },
		{ InputCode::Hyphen, "-" },
		{ InputCode::Apostrophe, "'" },
		{ InputCode::Grave, "`" },
		{ InputCode::ForwardSlash, "/" },
		{ InputCode::BackSlash, "\\" },
		{ InputCode::LBracket, "[" },
		{ InputCode::RBracket, "]" },

		// Maths
		{ InputCode::Decimal, ". (Keypad)" },
		{ InputCode::Add, "+ (Keypad)" },
		{ InputCode::Subtract, "- (Keypad)" },
		{ InputCode::Multiply, "* (Keypad)" },
		{ InputCode::Divide, "/ (Keypad)" },
		{ InputCode::Equals, "=" },
		{ InputCode::keypadEquals, "= (Keypad)" },

		// Locks
		{ InputCode::CapsLock, "Caps Lock" },
		{ InputCode::NumLock, "Num Lock" },
		{ InputCode::ScrollLock, "Scroll Lock" },

		// Modifiers
		{ InputCode::LShift, "Shift (Left)" },
		{ InputCode::RShift, "Shift (Right)" },
		{ InputCode::LControl, "Control (Left)" },
		{ InputCode::RControl, "Control (Right)" },
		{ InputCode::LAlt, "Alt (Left)" },
		{ InputCode::RAlt, "Alt (Right)" },
		{ InputCode::LSuper, "Super (Left)" },
		{ InputCode::RSuper, "Super (Right)" },

		// Function
		{ InputCode::f1, "f1" },
		{ InputCode::f2, "f2" },
		{ InputCode::f3, "f3" },
		{ InputCode::f4, "f4" },
		{ InputCode::f5, "f5" },
		{ InputCode::f6, "f6" },
		{ InputCode::f7, "f7" },
		{ InputCode::f8, "f8" },
		{ InputCode::f9, "f9" },
		{ InputCode::f10, "f10" },
		{ InputCode::f11, "f11" },
		{ InputCode::f12, "f12" },
		{ InputCode::f13, "f13" },
		{ InputCode::f14, "f14" },
		{ InputCode::f15, "f15" },
		{ InputCode::f16, "f16" },
		{ InputCode::f17, "f17" },
		{ InputCode::f18, "f18" },
		{ InputCode::f19, "f19" },
		{ InputCode::f20, "f20" },
		{ InputCode::f21, "f21" },
		{ InputCode::f22, "f22" },
		{ InputCode::f23, "f23" },
		{ InputCode::f24, "f24" },
		{ InputCode::f25, "f25" },

		// Numbers
		{ InputCode::num0, "0" },
		{ InputCode::num1, "1" },
		{ InputCode::num2, "2" },
		{ InputCode::num3, "3" },
		{ InputCode::num4, "4" },
		{ InputCode::num5, "5" },
		{ InputCode::num6, "6" },
		{ InputCode::num7, "7" },
		{ InputCode::num8, "8" },
		{ InputCode::num9, "9" },
		{ InputCode::keypad0, "0 (Keypad)" },
		{ InputCode::keypad1, "1 (Keypad)" },
		{ InputCode::keypad2, "2 (Keypad)" },
		{ InputCode::keypad3, "3 (Keypad)" },
		{ InputCode::keypad4, "4 (Keypad)" },
		{ InputCode::keypad5, "5 (Keypad)" },
		{ InputCode::keypad6, "6 (Keypad)" },
		{ InputCode::keypad7, "7 (Keypad)" },
		{ InputCode::keypad8, "8 (Keypad)" },
		{ InputCode::keypad9, "9 (Keypad)" },

		// Letters
		{ InputCode::A, "A" },
		{ InputCode::B, "B" },
		{ InputCode::C, "C" },
		{ InputCode::D, "D" },
		{ InputCode::E, "E" },
		{ InputCode::F, "F" },
		{ InputCode::G, "G" },
		{ InputCode::H, "H" },
		{ InputCode::I, "I" },
		{ InputCode::J, "J" },
		{ InputCode::K, "K" },
		{ InputCode::L, "L" },
		{ InputCode::M, "M" },
		{ InputCode::N, "N" },
		{ InputCode::O, "O" },
		{ InputCode::P, "P" },
		{ InputCode::Q, "Q" },
		{ InputCode::R, "R" },
		{ InputCode::S, "S" },
		{ InputCode::T, "T" },
		{ InputCode::U, "U" },
		{ InputCode::V, "V" },
		{ InputCode::W, "W" },
		{ InputCode::X, "X" },
		{ InputCode::Y, "Y" },
		{ InputCode::Z, "Z" },

		// Any Gamepad Input
		{ InputCode::GamepadA, "Gamepad A"},
		{ InputCode::GamepadB, "Gamepad B"},
		{ InputCode::GamepadX, "Gamepad X"},
		{ InputCode::GamepadY, "Gamepad Y"},
		{ InputCode::GamepadLB, "Gamepad Left Bumper"},
		{ InputCode::GamepadRB, "Gamepad Right Bumper"},
		{ InputCode::GamepadLM, "Gamepad Left Menu"},
		{ InputCode::GamepadRM, "Gamepad Right Menu"},
		{ InputCode::GamepadLS, "Gamepad Left Stick"},
		{ InputCode::GamepadRS, "Gamepad Right Stick"},
		{ InputCode::GamepadDPUp, "Gamepad D-Pad Up"},
		{ InputCode::GamepadDPRight, "Gamepad D-Pad Right"},
		{ InputCode::GamepadDPDown, "Gamepad D-Pad Down"},
		{ InputCode::GamepadDPLeft, "Gamepad D-Pad Left"},
	};
const std::map<const char*, InputCode> inputNameToCode = {
	// Mouse
	{ "Mouse Left", InputCode::MouseLeft },
	{ "Mouse Right", InputCode::MouseRight },
	{ "Mouse Middle", InputCode::MouseMiddle },
	{ "Mouse 3", InputCode::Mouse3 },
	{ "Mouse 4", InputCode::Mouse4 },
	{ "Mouse 5", InputCode::Mouse5 },
	{ "Mouse 6", InputCode::Mouse6 },
	{ "Mouse 7", InputCode::Mouse7 },

	// Special
	{ "Esc", InputCode::Esc },
	{ "Tab", InputCode::Tab },
	{ "Enter", InputCode::Enter },
	{ "Enter (Keypad)", InputCode::keypadEnter },
	{ "Backspace", InputCode::Backspace },
	{ "Delete", InputCode::Delete },
	{ "Insert", InputCode::Insert },
	{ "Pause", InputCode::Pause },
	{ "Print", InputCode::Print },
	{ "Menu", InputCode::Menu },

	// Navigation
	{ "Up", InputCode::Up },
	{ "Down", InputCode::Down },
	{ "Left", InputCode::Left },
	{ "Right", InputCode::Right },
	{ "Page Up", InputCode::PageUp },
	{ "Page Down", InputCode::PageDown },
	{ "Home", InputCode::Home },
	{ "End", InputCode::End },

	// Punctuation
	{ "Space", InputCode::Space },
	{ ",", InputCode::Comma },
	{ ".", InputCode::Period },
	{ ";", InputCode::Semicolon },
	{ "-", InputCode::Hyphen },
	{ "'", InputCode::Apostrophe },
	{ "`", InputCode::Grave },
	{ "/", InputCode::ForwardSlash },
	{ "\\", InputCode::BackSlash },
	{ "[", InputCode::LBracket },
	{ "]", InputCode::RBracket },

	// Maths
	{ ". (Keypad)", InputCode::Decimal },
	{ "+ (Keypad)", InputCode::Add },
	{ "- (Keypad)", InputCode::Subtract },
	{ "* (Keypad)", InputCode::Multiply },
	{ "/ (Keypad)", InputCode::Divide },
	{ "=", InputCode::Equals },
	{ "= (Keypad)", InputCode::keypadEquals },

	// Locks
	{ "Caps Lock", InputCode::CapsLock },
	{ "Num Lock", InputCode::NumLock },
	{ "Scroll Lock", InputCode::ScrollLock },

	// Modifiers
	{ "Shift (Left)", InputCode::LShift },
	{ "Shift (Right)", InputCode::RShift },
	{ "Control (Left)", InputCode::LControl },
	{ "Control (Right)", InputCode::RControl },
	{ "Alt (Left)", InputCode::LAlt },
	{ "Alt (Right)", InputCode::RAlt },
	{ "Super (Left)", InputCode::LSuper },
	{ "Super (Right)", InputCode::RSuper },

	// Function
	{ "f1", InputCode::f1 },
	{ "f2", InputCode::f2 },
	{ "f3", InputCode::f3 },
	{ "f4", InputCode::f4 },
	{ "f5", InputCode::f5 },
	{ "f6", InputCode::f6 },
	{ "f7", InputCode::f7 },
	{ "f8", InputCode::f8 },
	{ "f9", InputCode::f9 },
	{ "f10", InputCode::f10 },
	{ "f11", InputCode::f11 },
	{ "f12", InputCode::f12 },
	{ "f13", InputCode::f13 },
	{ "f14", InputCode::f14 },
	{ "f15", InputCode::f15 },
	{ "f16", InputCode::f16 },
	{ "f17", InputCode::f17 },
	{ "f18", InputCode::f18 },
	{ "f19", InputCode::f19 },
	{ "f20", InputCode::f20 },
	{ "f21", InputCode::f21 },
	{ "f22", InputCode::f22 },
	{ "f23", InputCode::f23 },
	{ "f24", InputCode::f24 },
	{ "f25", InputCode::f25 },

	// Numbers
	{ "0", InputCode::num0 },
	{ "1", InputCode::num1 },
	{ "2", InputCode::num2 },
	{ "3", InputCode::num3 },
	{ "4", InputCode::num4 },
	{ "5", InputCode::num5 },
	{ "6", InputCode::num6 },
	{ "7", InputCode::num7 },
	{ "8", InputCode::num8 },
	{ "9", InputCode::num9 },
	{ "0 (Keypad)", InputCode::keypad0 },
	{ "1 (Keypad)", InputCode::keypad1 },
	{ "2 (Keypad)", InputCode::keypad2 },
	{ "3 (Keypad)", InputCode::keypad3 },
	{ "4 (Keypad)", InputCode::keypad4 },
	{ "5 (Keypad)", InputCode::keypad5 },
	{ "6 (Keypad)", InputCode::keypad6 },
	{ "7 (Keypad)", InputCode::keypad7 },
	{ "8 (Keypad)", InputCode::keypad8 },
	{ "9 (Keypad)", InputCode::keypad9 },

	// Letters
	{ "A", InputCode::A },
	{ "B", InputCode::B },
	{ "C", InputCode::C },
	{ "D", InputCode::D },
	{ "E", InputCode::E },
	{ "F", InputCode::F },
	{ "G", InputCode::G },
	{ "H", InputCode::H },
	{ "I", InputCode::I },
	{ "J", InputCode::J },
	{ "K", InputCode::K },
	{ "L", InputCode::L },
	{ "M", InputCode::M },
	{ "N", InputCode::N },
	{ "O", InputCode::O },
	{ "P", InputCode::P },
	{ "Q", InputCode::Q },
	{ "R", InputCode::R },
	{ "S", InputCode::S },
	{ "T", InputCode::T },
	{ "U", InputCode::U },
	{ "V", InputCode::V },
	{ "W", InputCode::W },
	{ "X", InputCode::X },
	{ "Y", InputCode::Y },
	{ "Z", InputCode::Z },

	// Any Gamepad Input
	{ "Gamepad A", InputCode::GamepadA },
	{ "Gamepad B", InputCode::GamepadB },
	{ "Gamepad X", InputCode::GamepadX },
	{ "Gamepad Y", InputCode::GamepadY },
	{ "Gamepad Left Bumper", InputCode::GamepadLB },
	{ "Gamepad Right Bumper", InputCode::GamepadRB },
	{ "Gamepad Left Menu", InputCode::GamepadLM },
	{ "Gamepad Right Menu", InputCode::GamepadRM },
	{ "Gamepad Left Stick", InputCode::GamepadLS },
	{ "Gamepad Right Stick", InputCode::GamepadRS },
	{ "Gamepad D-Pad Up", InputCode::GamepadDPUp },
	{ "Gamepad D-Pad Right", InputCode::GamepadDPRight },
	{ "Gamepad D-Pad Down", InputCode::GamepadDPDown },
	{ "Gamepad D-Pad Left", InputCode::GamepadDPLeft },
};

Input::Input()
{
	downInputs = new std::set<InputCode>();
	pressedInputs = new std::set<InputCode>();
	releasedInputs = new std::set<InputCode>();

	for (uint i = GLFW_JOYSTICK_1; i <= GLFW_JOYSTICK_LAST; i++)
	{
		bool present = false;
		if (glfwJoystickIsGamepad(i) == GLFW_TRUE)
		{
			present = true;
			Debug::Log("Gamepad ", i, " is present on startup.");
		}
		presentGamepads.push_back(present);
	}

	GLFWwindow* window = glfwGetCurrentContext();
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, CursorPosCallback);
	glfwSetMouseButtonCallback(window, MouseButtonCallback);
	glfwSetScrollCallback(window, ScrollCallback);
	glfwSetJoystickCallback(GamepadConnectionCallback);
}
Input::~Input()
{
	del(downInputs);
	del(pressedInputs);
	del(releasedInputs);
}

bool Input::GetInputDown(InputCode inputCode) const noexcept
{
	return downInputs->count(inputCode) > 0;
}
bool Input::GetInputPressed(InputCode inputCode) const noexcept
{
	return pressedInputs->count(inputCode) > 0;
}
bool Input::GetInputReleased(InputCode inputCode) const noexcept
{
	return releasedInputs->count(inputCode) > 0;
}

void Input::Update()
{
	pressedInputs->clear();
	releasedInputs->clear();

	AppInfo::input->cursorMovement = vec2(0, 0);
	AppInfo::input->scrollInput = vec2(0, 0);

	std::array<bool, 14> globalInputs = std::array<bool, 14>();
	globalInputs.fill(false);

	// Get Gamepad Inputs
	for (uint g = 0; g < (uint)presentGamepads.size(); g++)
	{
		if (presentGamepads[g])
		{
			int buttonCount = 0;
			const unsigned char* buttons = glfwGetJoystickButtons(g, &buttonCount);
			for (uint b = 0; b < (uint)buttonCount; b++)
			{
				InputCode globalInputCode = InputCode((int)InputCode::GamepadFirst + b);
				InputCode localInputCode = InputCode((int)globalInputCode + ((g + 1) * 100));

				switch (buttons[b])
				{
				case GLFW_PRESS:
					globalInputs[b] = true;
					if (!GetInputDown(localInputCode))
					{
						downInputs->insert(localInputCode);
						pressedInputs->insert(localInputCode);
						Debug::Log("Input \"", Input::GetNameFromCode(globalInputCode), "\" was pressed on gamepad ", g);
					}
					break;
				case GLFW_RELEASE:
					if (GetInputDown(localInputCode))
					{
						downInputs->erase(localInputCode);
						releasedInputs->insert(localInputCode);
						Debug::Log("Input \"", Input::GetNameFromCode(globalInputCode), "\" was released on gamepad ", g);
					}
					break;
				default: break;
				}
			}
		}
	}
	// Consolidate Input Info for if Multiple Gamepads Aren't Needed
	for (uint b = 0; b < (uint)globalInputs.size(); b++)
	{
		bool wasDown = GetInputDown(InputCode((int)InputCode::GamepadFirst + b));
		bool isDownNow = globalInputs[b];

		if (!wasDown && isDownNow)
		{
			downInputs->insert(InputCode((int)InputCode::GamepadFirst + b));
			pressedInputs->insert(InputCode((int)InputCode::GamepadFirst + b));
		}
		else if (wasDown && !isDownNow)
		{
			downInputs->erase(InputCode((int)InputCode::GamepadFirst + b));
			releasedInputs->insert(InputCode((int)InputCode::GamepadFirst + b));
		}
	}
}

const char* Input::GetNameFromCode(InputCode inputCode) noexcept
{
	return inputCodeToName.find(inputCode)->second;
}
InputCode Input::GetCodeFromName(const char* const inputName) noexcept
{
	return inputNameToCode.find(inputName)->second;
}



void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		AppInfo::input->pressedInputs->insert((InputCode)key);
		AppInfo::input->downInputs->insert((InputCode)key);
	}
	else if (action == GLFW_RELEASE)
	{
		AppInfo::input->releasedInputs->insert((InputCode)key);
		AppInfo::input->downInputs->erase((InputCode)key);
	}
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (!AppInfo::input->enabled) return;

	if (action == GLFW_PRESS)
	{
		AppInfo::input->pressedInputs->insert((InputCode)button);
		AppInfo::input->downInputs->insert((InputCode)button);
	}
	else
	{
		AppInfo::input->releasedInputs->insert((InputCode)button);
		AppInfo::input->downInputs->erase((InputCode)button);
	}
}

void ScrollCallback(GLFWwindow* window, double x, double y)
{
	if (!AppInfo::input->enabled) return;

	AppInfo::input->scrollInput = vec2(x, y);
}

void CursorPosCallback(GLFWwindow* window, double x, double y)
{
	if (!AppInfo::input->enabled) return;

	vec2 newPos(x, y);
	AppInfo::input->cursorMovement = newPos - AppInfo::input->cursorPos;
	AppInfo::input->cursorPos = newPos;
}

void GamepadConnectionCallback(int id, int type)
{
	switch (type)
	{
	case GLFW_CONNECTED:
		if (glfwJoystickIsGamepad(id))
		{
			Debug::Log("Gamepad ", id, " Connected");
			AppInfo::input->presentGamepads[id] = true;
		}
		break;
	case GLFW_DISCONNECTED:
		if (AppInfo::input->presentGamepads[id] == true)
		{
			Debug::Log("Gamepad ", id, " Disconnected");
			AppInfo::input->presentGamepads[id] = false;
		}
		break;
	default:
		Debug::LogWarning("Undefined Controller behaviour (Controller ", id, ")", locationinfo);
		break;
	}
}
