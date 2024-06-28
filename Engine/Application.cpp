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
		Debug::LogSubtle("Setting up application...");
		int returnCode = Setup();
		if (returnCode != 0)
		{
			Debug::LogError(LogID::ERR001, "Application setup failed", locationinfo "\n", "dfhskf");
			return returnCode;
		}
		Debug::LogSubtle("Application setup successful\n");
	}

	Debug::LogSubtle("Initialising application...");
	Initialise();
	Debug::LogSubtle("Initialisation successful\n");

	if (Camera::main == nullptr)
	{
		Debug::LogWarning(LogID::WRN001, "A default was created to avoid crashing but the application will not behave as expected. ", locationinfo);
		Camera::main = GameObject::Instantiate<Camera>();
	}

	GameLoop();

	Debug::LogSubtle("Terminating application...");
	Close();
	Debug::LogSubtle("Application termination successful\n");

	return 0;
}



void Application::Initialise() { }

void Application::FixedUpdate() { } 
void Application::Update() { }
void Application::Draw() { }

bool Application::OnClose() { return true; }


int Application::Setup()
{
	if (!glfwInit())
	{
		Debug::LogError(LogID::ERR902, "GLFW failed to initialise", locationinfo);
		return -1;
	}

	glfwWindowHint(GLFW_MAXIMIZED, GL_TRUE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#if _DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
	window = glfwCreateWindow(1600, 900, "Window", nullptr, nullptr);
	if (window == nullptr)
	{
		Debug::LogError(LogID::ERR902, "GLFW failed to create a window", locationinfo);
		glfwTerminate();
		return -1;
	}

	glfwSetWindowFocusCallback(window, WindowFocusCallback);

	glfwMakeContextCurrent(window);

	input = new Input();

	glfwSwapInterval(0); // Disables VSync

	if (!gladLoadGL())
	{
		Debug::LogError(LogID::ERR901, "GLAD failed to load OpenGL functions", locationinfo);
		return -1;
	}

#if _DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(GLErrorCallback, nullptr);
	// Disable opengl logs of these types
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
	// Disable opengl logs with these id's
	const uint idCount = 1; const uint idsToIgnore[idCount] =
	{
		131218U, // This id alerts every time a shader gets recompiled at runtime but we want to allow this
	};
	glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_PERFORMANCE, GL_DONT_CARE, idCount, idsToIgnore, GL_FALSE);

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
	bool applicationIsReadyToClose = false;

	while (!applicationIsReadyToClose)
	{
		while (isRunning && !glfwWindowShouldClose(window))
		{
			input->Update();
			glfwPollEvents();

			Time::Tick((float)glfwGetTime());
			timeDebt += Time::delta;
			while (timeDebt > Time::fixedStep)
			{
				FixedUpdate();
				timeDebt -= Time::fixedStep;
			}

			Update();

			Draw();

			glfwSwapBuffers(window);
		}

		applicationIsReadyToClose = OnClose();

		if (!applicationIsReadyToClose)
		{
			isRunning = true;
			glfwSetWindowShouldClose(window, false);
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
	case GL_DEBUG_SEVERITY_HIGH:		Debug::LogError(LogID::ERR901, GLErrorSource(source), GLErrorType(type), id, ": \"", message, locationinfo); break;
	case GL_DEBUG_SEVERITY_MEDIUM:	Debug::LogWarning(LogID::WRN901, GLErrorSource(source), GLErrorType(type), id, ": \"", message, locationinfo); break;
	case GL_DEBUG_SEVERITY_LOW:		Debug::LogWarning(LogID::WRN902, GLErrorSource(source), GLErrorType(type), id, ": \"", message, locationinfo); break;
	}
}
void GLFWErrorCallback(int code, const char* description)
{
	Debug::LogError(LogID::ERR902, code, ": ", description, locationinfo);
}
