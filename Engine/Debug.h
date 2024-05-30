#pragma once

#include <string>
using std::string;

#include <initializer_list>
typedef std::initializer_list<string> StringParams;

#include <map>

#include <fstream>

#include "GeneralMacros.h"

#include "LineDrawer.h"

class Debug
{
public:
	LineDrawer lines;

	Debug() noexcept;
	~Debug() noexcept;
	uncopyable(Debug)
	immovable(Debug)
	
	mappedenumi(LogType, unsigned char, public,
		Message,
		Warning,
		Error,
		Subtle,
	)
	#pragma region LogID enum
	mappedenumi(LogID, unsigned int, public,
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
		ERR001,		// Setup Failure
		ERR002,		// Initialisation Failure

		// Files
		ERR100,		// Failed to Load Critical File
		ERR101,		// Failed to Load a Default Asset

		// Data & Serialisation
		ERR150,		// Fatal Data Failure
		ERR151,		// Unspecified Case for TypeID

		// 3rd Party
		ERR901,		// OpenGL Error
		ERR902,		// GLFW Error
	)
	#pragma endregion

	struct DebugLog
	{
		string message;
		LogID id;
		LogType type;

		DebugLog(LogType typeInit = LogType::Message, LogID idInit = LogID::Undefined) :
			type(typeInit),
			id(idInit) {}
		DebugLog(string messageInit, LogType typeInit = LogType::Message, LogID idInit = LogID::Undefined) :
			message(messageInit),
			type(typeInit),
			id(idInit) {}
		DebugLog(StringParams messageInit, LogType typeInit = LogType::Message, LogID idInit = LogID::Undefined) :
			type(typeInit),
			id(idInit)
			{ for (string str : messageInit) { message += str; } }
	};

	virtual DebugLog Log(const StringParams& message, const LogType type = Message, const LogID id = Undefined);
	virtual DebugLog Log(const string& message, const LogType type = Message, const LogID id = Undefined);
	virtual DebugLog Log(const LogType type, const LogID id = Undefined);

	static string GetLogAsString(const DebugLog& log) noexcept;

protected:
	std::ofstream* outputFile;
};

extern Debug* debug;