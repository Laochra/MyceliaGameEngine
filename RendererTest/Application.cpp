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

	GameLoop();

	Close();

	return 0;
}

void Application::Initialise()
{
	Gizmos::create(100000, 10000, 0, 0);
}

void Application::FixedUpdate()
{

}

void Application::Update()
{
	camera.Update();
}

void Application::Draw()
{
	Gizmos::clear();

	Gizmos::addTransform(glm::mat4(1));

	vec4 white(1);
	vec4 black(0, 0, 0, 1);

	for (int i = 0; i < 21; i++)
	{
		Gizmos::addLine(vec3(-10 + i, 0, 10), vec3(-10 + i, 0, -10), i == 10 ? white : black);

		Gizmos::addLine(vec3(-10, 0, -10 + i), vec3(10, 0, -10 + i), i == 10 ? white : black);
	}

	int w = 0, h = 0;
	glfwGetWindowSize(window, &w, &h);

	Gizmos::draw(camera.GetProjectionMatrix(w, h) * camera.GetViewMatrix());
}



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
	//glfwSetMouseButtonCallback(window, FUNCTION);
	//glfwSetScrollCallback(window, FUNCTION);

	glfwMakeContextCurrent(window);

	if (!gladLoadGL())
	{
		std::cout << std::endl << "GLAD failed to load OpenGL functions";
		return -1;
	}

	glClearColor(0.9f, 0.7f, 0.9f, 1);
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
	Gizmos::destroy();

	glfwTerminate();
}