#pragma once

class Input;
class Debug;
struct GLFWwindow;

enum class AppState
{
	Playing,
	Paused,
	Editor
};

class AppInfo
{
public:
	static AppState		state;
	static Input*			input;
	static Debug*			debug;
	static GLFWwindow*	window;
};
