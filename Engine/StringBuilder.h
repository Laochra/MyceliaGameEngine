#pragma once

#include <string>

#include "LogID.h"

std::string ValueAsString(char* value);
std::string ValueAsString(const char* value);
std::string ValueAsString(std::string value);
std::string ValueAsString(char value);

std::string ValueAsString(bool value);

std::string ValueAsString(signed int value);
std::string ValueAsString(unsigned int value);
std::string ValueAsString(signed long value);
std::string ValueAsString(unsigned long value);
std::string ValueAsString(signed long long value);
std::string ValueAsString(unsigned long long value);
std::string ValueAsString(float value);
std::string ValueAsString(double value);
std::string ValueAsString(long double value);

//std::string ValueAsString(LogID value);

template<typename Arg1>
std::string BuildString(Arg1 arg1)
{
	return ValueAsString(arg1);
}
template<typename Arg1, typename... Args>
std::string BuildString(Arg1 arg1, Args... args)
{
	return ValueAsString(arg1) + BuildString(args...);
}

template<typename RequiredParam, typename... OptionalParams>
struct StringBuilder
{
	std::string value;

	StringBuilder(RequiredParam requiredParam, OptionalParams... optionalParam)
	{
		value = BuildString(requiredParam, optionalParam...);
	}
};
