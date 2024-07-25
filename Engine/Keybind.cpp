#include "Input.h"

#include "GeneralMacros.h"
#include "AppInfo.h"

Keybind::Keybind() noexcept
{
    keys = new std::vector<KeyCode>();
}
Keybind::Keybind(KeyCode keyCode) noexcept
{
    keys = new std::vector<KeyCode>();

    Bind(keyCode);
}
Keybind::~Keybind() noexcept
{
    del(keys);
}

bool Keybind::down() const noexcept
{
    for (int i = 0; i < keys->size(); i++)
    {
        if (AppInfo::input->GetKeyDown((*keys)[i]))
        {
            return true;
        }
    }

    return false;
}
bool Keybind::pressed() const noexcept
{
    for (int i = 0; i < keys->size(); i++)
    {
        if (AppInfo::input->GetKeyPressed((*keys)[i]))
        {
            return true;
        }
    }

    return false;
}
bool Keybind::released() const noexcept
{
    for (int i = 0; i < keys->size(); i++)
    {
        if (AppInfo::input->GetKeyReleased((*keys)[i]))
        {
            return true;
        }
    }

    return false;
}


void Keybind::Bind(KeyCode keyCode) noexcept
{
    keys->push_back(keyCode);
}
void Keybind::Bind(std::vector<KeyCode> keyCodes) noexcept
{
    for (int i = 0; i < keyCodes.size(); i++)
    {
        Bind(keyCodes[i]);
    }
}
void Keybind::Unbind(KeyCode keyCode) noexcept
{
    for (int i = 0; i < keys->size(); i++)
    {
        if ((*keys)[i] == keyCode)
        {
            keys->erase(keys->begin() + i);
            break;
        }
    }
}
void Keybind::ClearBinds() noexcept
{
    keys->clear();
}