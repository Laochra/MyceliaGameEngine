#pragma once

#include <iostream>
#include <string>

#include "Gizmos.h"
#include <glm.hpp>
#include <ext.hpp>

using glm::vec3;
using glm::vec4;
using glm::mat4;

#define GLFW_INCLUDE_NONE
#include "glfw3.h"
#include "glad.h"

#include "Camera.h"
#include "FreeCamera.h"
#include "OrbitalCamera.h"

class Application
{
public:
	bool isRunning = false;
	FreeCamera camera;

	int Run();

	Application() noexcept
	{

	}

protected:

	GLFWwindow* window = nullptr;

	virtual void Initialise();
	virtual void FixedUpdate();
	virtual void Update();
	virtual void Draw();

private:
	int Setup();

	void GameLoop();

	void Close();
};