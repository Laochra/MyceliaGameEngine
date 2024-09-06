#include "Input.h"

#include "GeneralMacros.h"
#include "AppInfo.h"

#include "Debug.h"

InputAxis::InputAxis(InputCode negative, InputCode positive) noexcept
{
	Bind(negative, positive);
}
InputAxis::InputAxis(InputCode axis) noexcept
{
	BindAxis(axis);
}
InputAxis::InputAxis(InputCode negative, InputCode positive, InputCode axis) noexcept
{
	Bind(negative, positive, axis);
}
InputAxis::InputAxis(std::vector<InputCode> negatives, std::vector<InputCode> positives) noexcept
{
	Bind(negatives, positives);
}
InputAxis::InputAxis(std::vector<InputCode> axes) noexcept
{
	BindAxis(axes);
}
InputAxis::InputAxis(std::vector<InputCode> negatives, std::vector<InputCode> positives, std::vector<InputCode> axes) noexcept
{
	Bind(negatives, positives, axes);
}
InputAxis::InputAxis(const InputAxis& other) noexcept
{
	positiveBinds = other.positiveBinds;
	negativeBinds = other.negativeBinds;
	axisBinds = other.axisBinds;
}
InputAxis& InputAxis::operator=(const InputAxis& other) noexcept
{
	positiveBinds = other.positiveBinds;
	negativeBinds = other.negativeBinds;
	axisBinds = other.axisBinds;

	return *this;
}

float InputAxis::value() const noexcept
{
	bool positive = false;
	bool negative = false;

	for (InputCode positiveBind : positiveBinds)
	{
		if (AppInfo::input->GetInputDown(positiveBind))
		{
			positive = true;
			break;
		}
	}
	for (InputCode negativeBind : negativeBinds)
	{
		if (AppInfo::input->GetInputDown(negativeBind))
		{
			negative = true;
			break;
		}
	}
	float currentAxis = (positive ? 1.0f : 0.0f) - (negative ? 1.0f : 0.0f);

	if (currentAxis == 0.0f)
	{
		for (InputCode axisBind : axisBinds)
		{
			float axis = AppInfo::input->GetAxis(axisBind);
			if (abs(axis) > abs(currentAxis))
			{
				currentAxis = axis;
			}
		}
	}

	return currentAxis;
}

InputAxis::operator float() const noexcept
{
	return value();
}


void InputAxis::BindPositive(InputCode inputCode) noexcept
{
	positiveBinds.push_back(inputCode);
}
void InputAxis::BindPositive(std::vector<InputCode> inputCodes) noexcept
{
	for (InputCode inputCode : inputCodes)
	{
		BindPositive(inputCode);
	}
}
void InputAxis::BindNegative(InputCode inputCode) noexcept
{
	negativeBinds.push_back(inputCode);
}
void InputAxis::BindNegative(std::vector<InputCode> inputCodes) noexcept
{
	for (InputCode inputCode : inputCodes)
	{
		BindNegative(inputCode);
	}
}
void InputAxis::BindAxis(InputCode inputCode) noexcept
{
	if (IsAxis(inputCode))
	{
		axisBinds.push_back(inputCode);
	}
	else
	{
		Debug::LogWarning(functionname " should only be passed Axis InputCodes");
	}
}
void InputAxis::BindAxis(std::vector<InputCode> inputCodes) noexcept
{
	for (InputCode inputCode : inputCodes)
	{
		BindAxis(inputCode);
	}
}
void InputAxis::Bind(InputCode negative, InputCode positive) noexcept
{
	BindNegative(negative);
	BindPositive(positive);
}
void InputAxis::Bind(InputCode negative, InputCode positive, InputCode axis) noexcept
{
	BindNegative(negative);
	BindPositive(positive);
	BindAxis(axis);
}
void InputAxis::Bind(std::vector<InputCode> negatives, std::vector<InputCode> positives) noexcept
{
	BindNegative(negatives);
	BindPositive(positives);
}

void InputAxis::Bind(std::vector<InputCode> negatives, std::vector<InputCode> positives, std::vector<InputCode> axes) noexcept
{
	BindNegative(negatives);
	BindPositive(positives);
	BindAxis(axes);
}

void InputAxis::UnbindPositive(InputCode inputCode) noexcept
{
	for (std::vector<InputCode>::iterator it = positiveBinds.begin(); it < positiveBinds.end(); it++)
	{
		if (*it == inputCode)
		{
			positiveBinds.erase(it);
			break;
		}
	}
}
void InputAxis::UnbindNegative(InputCode inputCode) noexcept
{
	for (std::vector<InputCode>::iterator it = negativeBinds.begin(); it < negativeBinds.end(); it++)
	{
		if (*it == inputCode)
		{
			negativeBinds.erase(it);
			break;
		}
	}
}
void InputAxis::UnbindAxis(InputCode inputCode) noexcept
{
	for (std::vector<InputCode>::iterator it = axisBinds.begin(); it < axisBinds.end(); it++)
	{
		if (*it == inputCode)
		{
			axisBinds.erase(it);
			break;
		}
	}
}

void InputAxis::ClearPositiveBinds() noexcept
{
	positiveBinds.clear();
}
void InputAxis::ClearNegativeBinds() noexcept
{
	negativeBinds.clear();
}
void InputAxis::ClearAxisBinds() noexcept
{
	axisBinds.clear();
}
void InputAxis::ClearAllBinds() noexcept
{
	ClearPositiveBinds();
	ClearNegativeBinds();
	ClearAxisBinds();
}

const std::vector<InputCode>& InputAxis::GetPositiveBinds() const noexcept
{
	return positiveBinds;
}
const std::vector<InputCode>& InputAxis::GetNegativeBinds() const noexcept
{
	return negativeBinds;
}
const std::vector<InputCode>& InputAxis::GetAxisBinds() const noexcept
{
	return axisBinds;
}
