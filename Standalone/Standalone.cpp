#include "Standalone.h"
#include "AppInfo.h"

#include "Input.h"

#include "Camera.h"
#include "LightingManager.h"
#include "AudioManager.h"

#include "Mesh.h"

#include "Scene.h"

#include "Debug.h"

void Standalone::Initialise()
{
	glfwSetFramebufferSizeCallback(AppInfo::window, ScreenResizeCallback);
	glfwGetFramebufferSize(AppInfo::window, &AppInfo::screenWidth, &AppInfo::screenHeight);

	AppInfo::gameCamera = GameObject3D::Instantiate<Camera>(vec3(0.0f));

	LightingManager::ambientLight = Light(vec3(0.1f, 0.1f, 0.1f));
	LightingManager::directionalLight = DirectionalLight(vec3(1.0f, 1.0f, 1.0f), glm::normalize(vec3(-0.1f, -1, -1)));

	AppInfo::input->enabled = true;

	game->Initialise();
	game->OnStart();

	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	glfwSetInputMode(AppInfo::window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetWindowMonitor(AppInfo::window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
	AppInfo::fullscreen = true;
}

void Standalone::FixedUpdate()
{
	Updater::CallFixedUpdate();
	game->FixedUpdate();
}

void Standalone::Update()
{
	AppInfo::input->screenCursorPos = vec2(AppInfo::input->cursorPos.x, AppInfo::screenHeight - AppInfo::input->cursorPos.y);
	Updater::CallUpdate();
	game->Update();

	if (gameStatus == GameStatus::Restarting)
	{
		game->OnStop();
		game->OnStart();
		gameStatus = GameStatus::Running;
	}
	else if (gameStatus == GameStatus::Quitting)
	{
		isRunning = false;
	}

	if (AppInfo::input->GetInputPressed(InputCode::Tab))
	{
		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		if (AppInfo::fullscreen)
		{
			glfwSetInputMode(AppInfo::window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			glfwSetWindowMonitor(AppInfo::window, nullptr, 0, 0, mode->width, mode->height, mode->refreshRate);
			glfwMaximizeWindow(AppInfo::window);
		}
		else
		{
			glfwSetInputMode(AppInfo::window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			glfwSetWindowMonitor(AppInfo::window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
		}

		AppInfo::fullscreen = !AppInfo::fullscreen;
	}
}

void Standalone::Draw()
{
	if (AppInfo::screenWidth <= 0 || AppInfo::screenHeight <= 0) return;

	game->Draw();
}

bool Standalone::OnClose()
{
	game->OnStop();
	game->OnClose();

	return true;
}

void ScreenResizeCallback(GLFWwindow* window, int width, int height)
{
	AppInfo::screenWidth = width;
	AppInfo::screenHeight = height;
}
