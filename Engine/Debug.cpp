#include "Debug.h"

#include <fstream>

#ifdef _DEBUG
#include <iostream>
#endif

#if defined(_WIN32) || defined(_WIN64)
#include "Windows.h"
HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
#define LIGHTGREY 7
#define YELLOW 14
#define RED 12
#define SetConsoleTextColour(colour) SetConsoleTextAttribute(console, colour);
#endif

Debug* debug;

Debug::Debug() noexcept
{
	outputFile = new std::ofstream("Debug.log");
	outputFile->close();
}

Debug::~Debug() noexcept
{
	delete outputFile;
}

Debug::DebugLog Debug::Log(StringParams message, LogType type, LogID id)
{
	if (id == Undefined)
	{
		switch (type)
		{
		case Warning:	id = WRN000;	break;
		case Error:		id = ERR000;	break;
		}
	}

	DebugLog log;
	log.type = type;
	log.id = id;
	for (string str : message) { log.message += str; }

#ifdef _DEBUG
#if defined(_WIN32) || defined(_WIN64)
	switch (type)
	{
	case Warning:	SetConsoleTextColour(YELLOW);	break;
	case Error:		SetConsoleTextColour(RED);	break;
	}
#endif

	if (type == Message) { std::cout << log.message << '\n'; }
	else { std::cout << LogIDMap[id] << ": " << log.message << '\n'; }

#if defined(_WIN32) || defined(_WIN64)
	SetConsoleTextColour(LIGHTGREY);
#endif
#endif

	outputFile->open("Debug.log", std::ios::app);
	if (type == Message) { *outputFile << log.message << '\n'; }
	else { *outputFile << LogIDMap[id] << ": " << log.message << '\n'; }
	outputFile->close();

	return log;
}
Debug::DebugLog Debug::Log(LogType type, LogID id) { return Log(StringParams(), type, id); }

string Debug::GetLogAsString(Debug::DebugLog log) noexcept
{
	if (debug == nullptr) return "debug is nullptr";

	string result;
	if (log.type != Message)
	{
		result.append(debug->LogIDMap[log.id]);
		result.append(": ");
	}
	result.append(log.message);
	return result;
}