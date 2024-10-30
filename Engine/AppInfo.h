#pragma once

class Input;
class Debug;
struct GLFWwindow;
class Camera;

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
	static const char*	name;

	static int				screenWidth;
	static int				screenHeight;
	static bool				screenSizeJustChanged;
	static Camera*			gameCamera;
	static Camera*			editorCamera;
	static Camera*&		ActiveCamera();
};
