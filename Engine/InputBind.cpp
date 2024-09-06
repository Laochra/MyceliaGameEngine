#include "Input.h"

#include "GeneralMacros.h"
#include "AppInfo.h"

InputBind::InputBind(InputCode inputCode) noexcept
{
	Bind(inputCode);
}
InputBind::InputBind(std::vector<InputCode> inputCodes) noexcept
{
	for (InputCode inputCode : inputCodes)
	{
		Bind(inputCode);
	}
}

InputBind::InputBind(const InputBind& other) noexcept
{
	binds = other.binds;
}
InputBind& InputBind::operator=(const InputBind& other) noexcept
{
	binds = other.binds;
	return *this;
}

bool InputBind::down() const noexcept
{
	for (InputCode bind : binds)
	{
		if (AppInfo::input->GetInputDown(bind))
		{
			return true;
		}
	}

	return false;
}
bool InputBind::pressed() const noexcept
{
	for (InputCode bind : binds)
	{
		if (AppInfo::input->GetInputPressed(bind))
		{
			return true;
		}
	}

	return false;
}
bool InputBind::released() const noexcept
{
	for (InputCode bind : binds)
	{
		if (AppInfo::input->GetInputReleased(bind))
		{
			return true;
		}
	}

	return false;
}

InputBind::operator bool() const noexcept
{
	return down();
}


void InputBind::Bind(InputCode inputCode) noexcept
{
	binds.push_back(inputCode);
}
void InputBind::Bind(std::vector<InputCode> inputCodes) noexcept
{
	for (InputCode inputCode : inputCodes)
	{
		Bind(inputCode);
	}
}
void InputBind::Unbind(InputCode inputCode) noexcept
{
	for (std::vector<InputCode>::iterator it = binds.begin(); it < binds.end(); it++)
	{
		if (*it == inputCode)
		{
			binds.erase(it);
			break;
		}
	}
}
void InputBind::ClearBinds() noexcept
{
	binds.clear();
}

const std::vector<InputCode>& InputBind::GetBinds() const noexcept
{
	return binds;
}
