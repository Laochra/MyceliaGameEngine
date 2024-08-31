#include "Input.h"

#include "GeneralMacros.h"
#include "AppInfo.h"

InputAxis::InputAxis() noexcept
{
	positiveBinds = new std::vector<InputCode>();
	negativeBinds = new std::vector<InputCode>();
}
InputAxis::InputAxis(InputCode negative, InputCode positive) noexcept
{
	positiveBinds = new std::vector<InputCode>();
	negativeBinds = new std::vector<InputCode>();

	BindPair(negative, positive);
}
InputAxis::~InputAxis() noexcept
{
	del(positiveBinds);
	del(negativeBinds);
}

float InputAxis::value() const noexcept
{
	bool positive = false;
	bool negative = false;

	for (int i = 0; i < positiveBinds->size(); i++)
	{
		if (AppInfo::input->GetInputDown((*positiveBinds)[i]))
		{
			positive = true;
			break;
		}
	}
	for (int i = 0; i < positiveBinds->size(); i++)
	{
		if (AppInfo::input->GetInputDown((*positiveBinds)[i]))
		{
			negative = true;
			break;
		}
	}

	return (positive ? 1.0f : 0.0f) - (negative ? 1.0f : 0.0f);
}


void InputAxis::BindPositive(InputCode inputCode) noexcept
{
	positiveBinds->push_back(inputCode);
}
void InputAxis::BindPositive(std::vector<InputCode> inputCodes) noexcept
{
	for (int i = 0; i < inputCodes.size(); i++)
	{
		BindPositive(inputCodes[i]);
	}
}
void InputAxis::BindNegative(InputCode inputCode) noexcept
{
	negativeBinds->push_back(inputCode);
}
void InputAxis::BindNegative(std::vector<InputCode> inputCodes) noexcept
{
	for (int i = 0; i < inputCodes.size(); i++)
	{
		BindNegative(inputCodes[i]);
	}
}
void InputAxis::BindPair(InputCode negative, InputCode positive) noexcept
{
	negativeBinds->push_back(negative);
	positiveBinds->push_back(positive);
}

void InputAxis::UnbindPositive(InputCode inputCode) noexcept
{
	for (int i = 0; i < positiveBinds->size(); i++)
	{
		if ((*positiveBinds)[i] == inputCode)
		{
			positiveBinds->erase(positiveBinds->begin() + i);
			break;
		}
	}
}
void InputAxis::UnbindNegative(InputCode inputCode) noexcept
{
	for (int i = 0; i < negativeBinds->size(); i++)
	{
		if ((*negativeBinds)[i] == inputCode)
		{
			negativeBinds->erase(negativeBinds->begin() + i);
			break;
		}
	}
}

void InputAxis::ClearPositiveBinds() noexcept
{
	positiveBinds->clear();
}
void InputAxis::ClearNegativeBinds() noexcept
{
	negativeBinds->clear();
}
void InputAxis::ClearAllBinds() noexcept
{
	ClearPositiveBinds();
	ClearNegativeBinds();
}