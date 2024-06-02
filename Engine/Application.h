#pragma once

#include "Camera.h"

#include "GLIncludes.h"

class Application
{
public:
	bool isRunning = false;

	int Run();

	Application() noexcept { }


protected:
	GLFWwindow* window = nullptr;
	
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