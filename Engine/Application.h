#pragma once
#include "AppInfo.h"

#include "GLIncludes.h"

#include "Game.h"

enum class GameStatus
{
	Initialising,
	Running,
	Restarting,
	Quitting
};

class Application
{
public:
	bool isRunning = false;
	Game* game = nullptr;
	GameStatus gameStatus = GameStatus::Initialising;

	int Run();
	virtual void OnStateChange(AppState newState);

	Application(Game* const gameInit) noexcept { game = gameInit; }


protected:	
	virtual void Initialise();
	virtual void FixedUpdate();
	virtual void Update();
	virtual void Draw();
	virtual bool OnClose();

private:
	int Setup();
	void GameLoop();
	void Close();
};

extern bool applicationFocused;
void WindowFocusCallback(GLFWwindow* window, int focused);
void GLErrorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
void GLFWErrorCallback(int code, const char* description);