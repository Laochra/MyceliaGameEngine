#include "Application.h"

#include "Time.h"
#include "Input.h"
#include "Updater.h"

bool applicationFocused = true;

int Application::Run()
{
	isRunning = true;
	
	{
		int returnCode = Setup();
		if (returnCode != 0) return returnCode;
	}

	Initialise();

	if (camera == nullptr)
	{
		std::cout << std::endl <<
			"No camera was set up in Initialise(), so a default camera was created." <<
			"Application will not behave as expected." << std::endl;
		camera = new Camera();
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
void Application::OnClose() { }


int Application::Setup()
{
	if (!glfwInit())
	{
		std::cout << std::endl << "GLFW failed to initialise";
		return -1;
	}

	window = glfwCreateWindow(1600, 900, "My First Window", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << std::endl << "GLFW failed to create a window";
		glfwTerminate();
		return -1;
	}

	input = new Input();

	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, CursorPosCallback);
	glfwSetMouseButtonCallback(window, MouseButtonCallback);
	glfwSetScrollCallback(window, ScrollCallback);

	glfwSetWindowFocusCallback(window, WindowFocusCallback);
	glfwSetErrorCallback(ErrorCallback);

	glfwMakeContextCurrent(window);

	if (!gladLoadGL())
	{
		std::cout << std::endl << "GLAD failed to load OpenGL functions";
		return -1;
	}

	glClearColor(0.8f, 0.75f, 0.85f, 1);
	glEnable(GL_DEPTH_TEST);

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
			camera->Update(); // remove this once camera is a gameobject
		}

		if (isRunning)
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			Draw();
			Updater::CallDraw();

			glfwSwapBuffers(window);
		}
	}
}

void Application::Close()
{
	delete input;

	glfwTerminate();
}



void WindowFocusCallback(GLFWwindow* window, int focused)
{
	//applicationFocused = focused ? true : false;
}

void ErrorCallback(int code, const char* description)
{
	std::cout << std::endl << "ERROR " << code << ": " << description << std::endl;
}