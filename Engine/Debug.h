#pragma once

#include <string>
using std::string;

#include <initializer_list>
typedef std::initializer_list<string> StringParams;

#include <map>

#include <fstream>

#include "GeneralMacros.h"

class Debug
{
public:
	Debug() noexcept;
	~Debug() noexcept;
	uncopyable(Debug)
	immovable(Debug)
	
	InternalMappedEnum(LogType, unsigned char, public, Message, Warning, Error)
	InternalMappedEnum(LogID, unsigned int, public,
		Undefined,	// Default ID


		///  Warnings  ///
		WRN000,		// Undefined Warning

		// Initialisation
		WRN001,		// Camera Not Initialised

		// Files
		WRN100,		// Failed to Load File
		WRN101,		// File Not Found
		WRN102,		// File Invalid
		WRN103,		// File Missing Data
		WRN104,		// Requested Default Doesn't Exist

		// Data & Serialisation
		WRN150,		// Data Failure
		WRN151,		// POD Data Size Mismatch

		// 3rd Party
		WRN901,		// OpenGL Warning
		WRN902,		// OpenGL Warning (Severe)


		///   Errors   ///
		ERR000,		// Undefined Error

		// 3rd Party
		ERR901,		// OpenGL Error
		ERR902,		// GLFW Error
	)

	struct DebugLog
	{
		string message;
		LogID id;
		LogType type;
	};

	virtual DebugLog Log(StringParams message, LogType type = Message, LogID id = Undefined);
	virtual DebugLog Log(LogType type, LogID id = Undefined);

	static string GetLogAsString(DebugLog log) noexcept;

protected:
	std::ofstream* outputFile;
};

extern Debug* debug;