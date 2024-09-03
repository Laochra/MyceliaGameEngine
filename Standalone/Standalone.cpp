#include "Standalone.h"
#include "AppInfo.h"

#include "Input.h"

#include "Camera.h"
#include "LightingManager.h"

#include "Mesh.h"

#include "Scene.h"

#include "Debug.h"

void Standalone::Initialise()
{
	glfwSetFramebufferSizeCallback(AppInfo::window, ScreenResizeCallback);
	glfwGetFramebufferSize(AppInfo::window, &AppInfo::screenWidth, &AppInfo::screenHeight);

	Scene::Open("Assets\\Scenes\\Sample.scene");

	AppInfo::gameCamera = GameObject3D::Instantiate<Camera>(vec3(-0.25f, 1.0f, 1.5f));

	LightingManager::ambientLight = Light(vec3(0.1f, 0.1f, 0.1f));
	LightingManager::directionalLight = DirectionalLight(vec3(1.0f, 1.0f, 1.0f), glm::normalize(vec3(-0.1f, -1, -1)));

	AppInfo::input->enabled = false;

	game->Initialise();
	game->OnStart();

	glfwSetWindowTitle(AppInfo::window, AppInfo::name);
}

void Standalone::FixedUpdate()
{
	Updater::CallFixedUpdate();
	game->FixedUpdate();
}

void Standalone::Update()
{
	Updater::CallUpdate();
	game->Update();
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
