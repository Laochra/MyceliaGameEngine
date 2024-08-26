#include "AppInfo.h"
#include "Application.h"
#include "Debug.h"
#include "Input.h"
#include "Camera.h"
#include "GLIncludes.h"

AppState		AppInfo::state;

Input*		AppInfo::input = nullptr;

Debug*		AppInfo::debug = nullptr;

GLFWwindow* AppInfo::window = nullptr;
const char* AppInfo::name = "Game Made in Mycelia Game Engine";

int			AppInfo::screenWidth = 0;
int			AppInfo::screenHeight = 0;
bool			AppInfo::screenSizeJustChanged = true;
Camera*     AppInfo::gameCamera = nullptr;
Camera*     AppInfo::editorCamera = nullptr;
Camera* AppInfo::ActiveCamera()
{
   switch (state)
   {
   case AppState::Editor:  return editorCamera;
   default:                return gameCamera;
   }
}

Application* AppInfo::application;

AppState AppInfo::GetState() noexcept
{
   return state;
}
bool AppInfo::CompareState(AppState stateToCheckFor) noexcept
{
   return stateToCheckFor == state;
}
void AppInfo::SetState(AppState newState) noexcept
{
   if (application != nullptr) application->OnStateChange(newState);
   state = newState;
}
