#include "Input.h"

#include "GeneralMacros.h"

#include "AppInfo.h"

Input::Input()
{
    downKeys = new std::set<KeyCode>();
    pressedKeys = new std::set<KeyCode>();
    releasedKeys = new std::set<KeyCode>();

    GLFWwindow* window = glfwGetCurrentContext();
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, CursorPosCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetScrollCallback(window, ScrollCallback);
}
Input::~Input()
{
    del(downKeys);
    del(pressedKeys);
    del(releasedKeys);
}

bool Input::GetKeyDown(KeyCode keyCode) const noexcept
{
    return downKeys->count(keyCode) > 0;
}
bool Input::GetKeyPressed(KeyCode keyCode) const noexcept
{
    return pressedKeys->count(keyCode) > 0;
}
bool Input::GetKeyReleased(KeyCode keyCode) const noexcept
{
    return releasedKeys->count(keyCode) > 0;
}

void Input::Update()
{
    pressedKeys->clear();
    releasedKeys->clear();

    AppInfo::input->cursorMovement = vec2(0, 0);
    AppInfo::input->scrollInput = vec2(0, 0);
}



void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        AppInfo::input->pressedKeys->insert((KeyCode)key);
        AppInfo::input->downKeys->insert((KeyCode)key);
    }
    else if (action == GLFW_RELEASE)
    {
        AppInfo::input->releasedKeys->insert((KeyCode)key);
        AppInfo::input->downKeys->erase((KeyCode)key);
    }
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (!AppInfo::input->enabled) return;

    if (action == GLFW_PRESS)
    {
        AppInfo::input->pressedKeys->insert((KeyCode)button);
        AppInfo::input->downKeys->insert((KeyCode)button);
    }
    else
    {
        AppInfo::input->releasedKeys->insert((KeyCode)button);
        AppInfo::input->downKeys->erase((KeyCode)button);
    }
}

void ScrollCallback(GLFWwindow* window, double x, double y)
{
    if (!AppInfo::input->enabled) return;

    AppInfo::input->scrollInput = vec2(x, y);
}

void CursorPosCallback(GLFWwindow* window, double x, double y)
{
    if (!AppInfo::input->enabled) return;

    vec2 newPos(x, y);
    AppInfo::input->cursorMovement = newPos - AppInfo::input->cursorPos;
    AppInfo::input->cursorPos = newPos;
}