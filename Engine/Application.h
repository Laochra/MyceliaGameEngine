#pragma once

#include <iostream>

#include "Camera.h"

#include "GLIncludes.h"

class Application
{
public:
	bool isRunning = false;

	Camera* camera = nullptr;

	int Run();

	Application() noexcept { }


protected:
	GLFWwindow* window = nullptr;
	
	virtual void Initialise();
	virtual void OnFrameStart();
	virtual void FixedUpdate();
	virtual void Update();
	virtual void Draw();
	virtual void OnClose();

private:
	int Setup();
	void GameLoop();
	void Close();
};

extern bool applicationFocused;
void WindowFocusCallback(GLFWwindow* window, int focused);
void ErrorCallback(int code, const char* description);