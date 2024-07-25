#include "AppInfo.h"
#include "Debug.h"
#include "Input.h"
#include "GLIncludes.h"

AppState		AppInfo::state;
Input*		AppInfo::input = nullptr;
Debug*		AppInfo::debug = nullptr;
GLFWwindow* AppInfo::window = nullptr;
