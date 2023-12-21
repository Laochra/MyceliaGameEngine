#pragma once

#include <iostream>
#include <string>

#define GLFW_INCLUDE_NONE
#include "glfw3.h"
#include "glad.h"
#include <glm.hpp>

#include "Camera.h"

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