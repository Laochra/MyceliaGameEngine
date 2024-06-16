#pragma once

#include <string>
using std::string;

#include <initializer_list>
typedef std::initializer_list<string> StringParams;

#include <map>

#include <fstream>

#include "GeneralMacros.h"

#include "LineDrawer.h"

#include "StackTimer.h"

#include "StringBuilder.h"

class Debug;
extern Debug* debug;

#pragma region LogID enum
mappedenum(LogID, unsigned int,
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
	WRN105,		// Invalid Load Path Given
	WRN120,		// Failed to Save File
	WRN121,		// Invalid Save Path Given

	// Data & Serialisation
	WRN150,		// Data Failure
	WRN151,		// POD Data Size Mismatch
	WRN152,		// Unknown Data Format Version

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
	
	struct DebugLog
	{
		string message;
		LogID id;
		LogType type;

		DebugLog(string messageInit, LogType typeInit = LogType::Message, LogID idInit = LogID::Undefined) :
			message(messageInit),
			type(typeInit),
			id(idInit) {}
	};

	template<typename Req, typename... Opt>
	static DebugLog Log(Req message, Opt... messageContinued)
	{
		return debug->LogImplementation(StringBuilder(message, messageContinued...).value, LogType::Message);
	}
	template<typename Req, typename... Opt>
	static DebugLog LogSubtle(Req message, Opt... messageContinued)
	{
		return debug->LogImplementation(StringBuilder(message, messageContinued...).value, LogType::Subtle);
	}

	template<typename Req, typename... Opt>
	static DebugLog LogWarning(Req message, Opt... messageContinued)
	{
		return debug->LogImplementation(StringBuilder(message, messageContinued...).value, LogType::Warning);
	}
	template<typename Req, typename... Opt>
	static DebugLog LogWarning(LogID warningCode, Req message, Opt... messageContinued)
	{
		return debug->LogImplementation(StringBuilder(message, messageContinued...).value, LogType::Warning, warningCode);
	}

	template<typename Req, typename... Opt>
	static DebugLog LogError(Req message, Opt... messageContinued)
	{
		return debug->LogImplementation(StringBuilder(message, messageContinued...).value, LogType::Error);
	}
	template<typename Req, typename... Opt>
	static DebugLog LogError(LogID errorCode, Req message, Opt... messageContinued)
	{
		return debug->LogImplementation(StringBuilder(message, messageContinued...).value, LogType::Error, errorCode);
	}

	static string GetLogAsString(const DebugLog& log) noexcept;

protected:
	std::ofstream* outputFile;

	virtual DebugLog LogImplementation(const string& message, const LogType type = Message, const LogID id = Undefined);
};
