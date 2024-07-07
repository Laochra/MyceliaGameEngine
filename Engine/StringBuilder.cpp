#include "StringBuilder.h"
#include <format>

// Uses value to construct string
std::string ValueAsString(char* value)
{
	return std::string(value);
}
std::string ValueAsString(const char* value)
{
	return std::string(value);
}
std::string ValueAsString(std::string value)
{
	return value;
}
std::string ValueAsString(char value)
{
	return std::string(&value, 1);
}

// Overridden behaviour
std::string ValueAsString(bool value)
{
	return std::string(value == true ? "true" : "false");
}
std::string ValueAsString(void* value)
{
	return "0x" + std::format("{:X}", (long long)value);
}

// Just uses std::to_string()
std::string ValueAsString(signed int value)
{
	return std::to_string(value);
}
std::string ValueAsString(unsigned int value)
{
	return std::to_string(value);
}
std::string ValueAsString(signed long value)
{
	return std::to_string(value);
}
std::string ValueAsString(unsigned long value)
{
	return std::to_string(value);
}
std::string ValueAsString(signed long long value)
{
	return std::to_string(value);
}
std::string ValueAsString(unsigned long long value)
{
	return std::to_string(value);
}
std::string ValueAsString(float value)
{
	return std::to_string(value);
}
std::string ValueAsString(double value)
{
	return std::to_string(value);
}
std::string ValueAsString(long double value)
{
	return std::to_string(value);
}

//std::string ValueAsString(LogID value)
//{
//	return std::string(value.name) + ": " + std::string(value.message);
//}
