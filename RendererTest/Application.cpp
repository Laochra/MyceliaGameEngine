#include "Application.h"

#include "Time.h"
#include "Input.h"

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
		Time::Tick(glfwGetTime());
		timeDebt += Time::delta;

		input->Update();

		glfwPollEvents();

		while (timeDebt > Time::fixedStep)
		{
			FixedUpdate();
			timeDebt -= Time::fixedStep;
		}

		if (isRunning) Update();
		
		camera->Update();

		if (isRunning)
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			Draw();

			glfwSwapBuffers(window);
		}
	}
}

void Application::Close()
{
	delete input;

	glfwTerminate();
}