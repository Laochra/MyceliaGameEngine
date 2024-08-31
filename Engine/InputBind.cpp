#include "Input.h"

#include "GeneralMacros.h"
#include "AppInfo.h"

InputBind::InputBind() noexcept
{
	binds = new std::vector<InputCode>();
}
InputBind::InputBind(InputCode keyCode) noexcept
{
	binds = new std::vector<InputCode>();

	Bind(keyCode);
}
InputBind::~InputBind() noexcept
{
	del(binds);
}

bool InputBind::down() const noexcept
{
	for (int i = 0; i < binds->size(); i++)
	{
		if (AppInfo::input->GetInputDown((*binds)[i]))
		{
			return true;
		}
	}

	return false;
}
bool InputBind::pressed() const noexcept
{
	for (int i = 0; i < binds->size(); i++)
	{
		if (AppInfo::input->GetInputPressed((*binds)[i]))
		{
			return true;
		}
	}

	return false;
}
bool InputBind::released() const noexcept
{
	for (int i = 0; i < binds->size(); i++)
	{
		if (AppInfo::input->GetInputReleased((*binds)[i]))
		{
			return true;
		}
	}

	return false;
}


void InputBind::Bind(InputCode inputCode) noexcept
{
	binds->push_back(inputCode);
}
void InputBind::Bind(std::vector<InputCode> inputCodes) noexcept
{
	for (int i = 0; i < inputCodes.size(); i++)
	{
		Bind(inputCodes[i]);
	}
}
void InputBind::Unbind(InputCode inputCode) noexcept
{
	for (int i = 0; i < binds->size(); i++)
	{
		if ((*binds)[i] == inputCode)
		{
			binds->erase(binds->begin() + i);
			break;
		}
	}
}
void InputBind::ClearBinds() noexcept
{
	binds->clear();
}