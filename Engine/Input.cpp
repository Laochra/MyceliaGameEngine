#include "Input.h"

#include "MemoryManagement.h"

Input* input = nullptr;

Input::Input()
{
    downKeys = new std::set<KeyCode>();
    pressedKeys = new std::set<KeyCode>();
    releasedKeys = new std::set<KeyCode>();
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

    input->cursorMovement = vec2(0, 0);
    input->scrollInput = vec2(0, 0);
}



void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        input->pressedKeys->insert((KeyCode)key);
        input->downKeys->insert((KeyCode)key);
    }
    else if (action == GLFW_RELEASE)
    {
        input->releasedKeys->insert((KeyCode)key);
        input->downKeys->erase((KeyCode)key);
    }
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (!input->enabled) return;

    if (action == GLFW_PRESS)
    {
        input->pressedKeys->insert((KeyCode)button);
        input->downKeys->insert((KeyCode)button);
    }
    else
    {
        input->releasedKeys->insert((KeyCode)button);
        input->downKeys->erase((KeyCode)button);
    }
}

void ScrollCallback(GLFWwindow* window, double x, double y)
{
    if (!input->enabled) return;

    input->scrollInput = vec2(x, y);
}

void CursorPosCallback(GLFWwindow* window, double x, double y)
{
    if (!input->enabled) return;

    vec2 newPos(x, y);
    input->cursorMovement = newPos - input->cursorPos;
    input->cursorPos = newPos;
}