#include "Application.h"

#include "TimeManager.h"
#include "Input.h"
#include "Updater.h"

#include "ShaderManager.h"
#include "MaterialManager.h"
#include "TextureManager.h"

bool applicationFocused = true;

int Application::Run()
{
	isRunning = true;
	
	{
		int returnCode = Setup();
		if (returnCode != 0) return returnCode;
	}

	Initialise();

	if (mainCamera == nullptr)
	{
		std::cout <<
			"\nNo camera was set up in Initialise(), so a default camera was created." <<
			"Application will not behave as expected.\n";
		mainCamera = GameObject::Instantiate<Camera>();
	}

	GameLoop();

	OnClose();
	Close();

	return 0;
}



void Application::Initialise() { }
void Application::OnFrameStart() { }

void Application::FixedUpdate() { } 
void Application::Update() { } 
void Application::Draw() { }
void Application::DrawGUI() { }
void Application::OnClose() { }


int Application::Setup()
{
	if (!glfwInit())
	{
		std::cout << "\nGLFW failed to initialise";
		return -1;
	}

	glfwWindowHint(GLFW_MAXIMIZED, GL_TRUE);
	window = glfwCreateWindow(1600, 900, "Window", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "\nGLFW failed to create a window";
		glfwTerminate();
		return -1;
	}

	input = new Input();
	shaderManager = new ShaderManager();
	materialManager = new MaterialManager();
	textureManager = new TextureManager();

	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, CursorPosCallback);
	glfwSetMouseButtonCallback(window, MouseButtonCallback);
	glfwSetScrollCallback(window, ScrollCallback);

	glfwSetWindowFocusCallback(window, WindowFocusCallback);
	glfwSetErrorCallback(ErrorCallback);

	glfwMakeContextCurrent(window);

	if (!gladLoadGL())
	{
		std::cout << "\nGLAD failed to load OpenGL functions";
		return -1;
	}

	return 0;
}

void Application::GameLoop()
{
	float timeDebt = 0;
	while (isRunning && !glfwWindowShouldClose(window))
	{
		input->Update();
		glfwPollEvents();

		OnFrameStart();
		Updater::CallOnFrameStart();

		Time::Tick(glfwGetTime());
		timeDebt += Time::delta;
		while (timeDebt > Time::fixedStep)
		{
			FixedUpdate();
			Updater::CallFixedUpdate();
			timeDebt -= Time::fixedStep;
		}

		if (isRunning)
		{
			Update();
			Updater::CallUpdate();
		}

		if (isRunning)
		{
			Draw();
			Updater::CallDraw();
			DrawGUI();

			glfwSwapBuffers(window);
		}
	}
}

void Application::Close()
{
	delete input;
	delete shaderManager;
	delete materialManager;
	delete textureManager;

	glfwTerminate();
}



void WindowFocusCallback(GLFWwindow* window, int focused)
{
	applicationFocused = focused;
}

void ErrorCallback(int code, const char* description)
{
	std::cout << "\nERROR " << code << ": " << description << "\n";
}