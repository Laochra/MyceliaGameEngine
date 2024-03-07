#pragma once

#include <string>
using std::string;

inline void AssignStringToCString(char*& destination, string& source)
{
	int bufferSize = source.size();
	destination = new char[bufferSize + 1];

	for (int i = 0; i < bufferSize; i++)
	{
		destination[i] = source[i];
	}
	destination[bufferSize] = '\0';
}