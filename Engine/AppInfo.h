#pragma once

class Input;
class Debug;
struct GLFWwindow;
class Camera;
class Application;

enum class AppState
{
	Playing,
	Paused,
	Editor
};

class AppInfo
{
public:
	static Input*			input;

	static Debug*			debug;

	static GLFWwindow*	window;
	static const char*	name;

	static int				screenWidth;
	static int				screenHeight;
	static bool				screenSizeJustChanged;
	static Camera*			gameCamera;
	static Camera*			editorCamera;
	static Camera*			ActiveCamera();

	static Application* application;

	static AppState GetState() noexcept;
	static bool CompareState(AppState stateToCheckFor) noexcept;
	static void SetState(AppState newState) noexcept;

private:
	static AppState		state;
};
