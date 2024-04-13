#include "Application.h"

#include "TimeManager.h"
#include "Input.h"
#include "Updater.h"

#include "ShaderManager.h"
#include "MaterialManager.h"
#include "TextureManager.h"
#include "MeshManager.h"

#include "Debug.h"

#include "GeneralMacros.h"

bool applicationFocused = true;

int Application::Run()
{
	isRunning = true;

	{
		debug->Log("Setting up application...");
		int returnCode = Setup();
		if (returnCode != 0)
		{
			debug->Log("Application setup failed\n", Debug::Error, Debug::ERR001);
			return returnCode;
		}
		debug->Log("Application setup successful\n");
	}

	debug->Log("Initialising application...");
	Initialise();
	debug->Log("Initialisation successful\n");

	if (mainCamera == nullptr)
	{
		debug->Log({ "No Camera was set up in Initialise(), so a default was created. Application will not behave as expected." }, Debug::Warning, Debug::WRN001);
		mainCamera = GameObject::Instantiate<Camera>();
	}

	GameLoop();

	debug->Log("Terminating application...");
	OnClose();
	Close();
	debug->Log("Application termination successful\n");

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
		debug->Log({ "GLFW failed to initialise" }, Debug::Error, Debug::ERR902);
		return -1;
	}

	glfwWindowHint(GLFW_MAXIMIZED, GL_TRUE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#if _DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
	window = glfwCreateWindow(1600, 900, "Window", nullptr, nullptr);
	if (window == nullptr)
	{
		debug->Log({ "GLFW failed to create a window" }, Debug::Error, Debug::ERR902);
		glfwTerminate();
		return -1;
	}

	input = new Input();

	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, CursorPosCallback);
	glfwSetMouseButtonCallback(window, MouseButtonCallback);
	glfwSetScrollCallback(window, ScrollCallback);

	glfwSetWindowFocusCallback(window, WindowFocusCallback);

	glfwMakeContextCurrent(window);

	glfwSwapInterval(0); // Disables VSync

	if (!gladLoadGL())
	{
		debug->Log({ "GLAD failed to load OpenGL functions" }, Debug::Error, Debug::ERR901);
		return -1;
	}

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

#if _DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(GLErrorCallback, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE); // Look into documentation for this
	glfwSetErrorCallback(GLFWErrorCallback);
#endif

	shaderManager = new ShaderManager();
	materialManager = new MaterialManager();
	textureManager = new TextureManager();
	meshManager = new MeshManager();

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

		Time::Tick((float)glfwGetTime());
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
	del(input);

	del(shaderManager);
	del(materialManager);
	del(textureManager);

	glfwTerminate();
}



void WindowFocusCallback(GLFWwindow* window, int focused)
{
	applicationFocused = focused;
}

const char* GLErrorSource(GLenum source)
{
	switch (source)
	{
	case GL_DEBUG_SOURCE_API:					return "OpenGL API ";
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:		return "Windows API ";
	case GL_DEBUG_SOURCE_SHADER_COMPILER:	return "Shader Compiler ";
	case GL_DEBUG_SOURCE_THIRD_PARTY:		return "Third Party ";
	case GL_DEBUG_SOURCE_APPLICATION:		return "Application ";
	default:											return "Other ";
	}
}
const char* GLErrorType(GLenum type)
{
	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:						return "General ";
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:	return "Deprecated Feature ";
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:		return "Undefined Behaviour ";
	case GL_DEBUG_TYPE_PORTABILITY:				return "Portability Issue ";
	case GL_DEBUG_TYPE_PERFORMANCE:				return "Performance ";
	case GL_DEBUG_TYPE_MARKER:						return "Annotation ";
	case GL_DEBUG_TYPE_PUSH_GROUP:				return "Group Push ";
	case GL_DEBUG_TYPE_POP_GROUP:					return "Group Pop ";
	default:												return "Other";
	}
}
const char* GLErrorSeverity(GLenum severity)
{
	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:				return "Error ";
	case GL_DEBUG_SEVERITY_MEDIUM:			return "Severe Warning ";
	case GL_DEBUG_SEVERITY_LOW:				return "Warning ";
	default:											return "Notification ";
	}
}
void GLErrorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:		debug->Log({ "OpenGL ", GLErrorSource(source), GLErrorType(type), std::to_string(id), ": \"", message }, Debug::Error,   Debug::ERR901); break;
	case GL_DEBUG_SEVERITY_MEDIUM:	debug->Log({ "OpenGL ", GLErrorSource(source), GLErrorType(type), std::to_string(id), ": \"", message }, Debug::Warning, Debug::WRN901); break;
	case GL_DEBUG_SEVERITY_LOW:		debug->Log({ "OpenGL ", GLErrorSource(source), GLErrorType(type), std::to_string(id), ": \"", message }, Debug::Warning, Debug::WRN902); break;
	}
}
void GLFWErrorCallback(int code, const char* description)
{
	debug->Log({ "GLFW ", std::to_string(code), ": ", description }, Debug::Error, Debug::ERR902);
}