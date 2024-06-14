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

	template<typename RequiredParam, typename... OptionalParams>
	struct MSG : public StringBuilder<RequiredParam, OptionalParams...>
	{
		MSG(RequiredParam requiredParam, OptionalParams... optionalParam) :
			StringBuilder<RequiredParam, OptionalParams...>(requiredParam, optionalParam...)
		{ }
	};
	template<typename RequiredParam, typename... OptionalParams>
	struct SUBTLE : public StringBuilder<RequiredParam, OptionalParams...>
	{
		SUBTLE(RequiredParam requiredParam, OptionalParams... optionalParam) :
			StringBuilder<RequiredParam, OptionalParams...>(requiredParam, optionalParam...)
		{ }
	};
	template<typename RequiredParam, typename... OptionalParams>
	struct WRN : public StringBuilder<RequiredParam, OptionalParams...>
	{
		WRN(RequiredParam requiredParam, OptionalParams... optionalParam) :
			StringBuilder<RequiredParam, OptionalParams...>(requiredParam, optionalParam...)
		{ }
	};
	template<typename RequiredParam, typename... OptionalParams>
	struct ERR : public StringBuilder<RequiredParam, OptionalParams...>
	{
		ERR(RequiredParam requiredParam, OptionalParams... optionalParam) :
			StringBuilder<RequiredParam, OptionalParams...>(requiredParam, optionalParam...)
		{ }
	};

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

	template<typename RequiredParam, typename... OptionalParams>
	static DebugLog Log(MSG<RequiredParam, OptionalParams...> stringBuilder)
	{
		return debug->LogImplementation(stringBuilder.value, LogType::Message, LogID::Undefined);
	}
	template<typename RequiredParam, typename... OptionalParams>
	static DebugLog Log(SUBTLE<RequiredParam, OptionalParams...> stringBuilder)
	{
		return debug->LogImplementation(stringBuilder.value, LogType::Subtle, LogID::Undefined);
	}
	template<typename RequiredParam, typename... OptionalParams>
	static DebugLog Log(WRN<RequiredParam, OptionalParams...> stringBuilder, LogID warningCode = LogID::WRN000)
	{
		return debug->LogImplementation(stringBuilder.value, LogType::Warning, warningCode);
	}
	template<typename RequiredParam, typename... OptionalParams>
	static DebugLog Log(ERR<RequiredParam, OptionalParams...> stringBuilder, LogID errorCode = LogID::ERR000)
	{
		return debug->LogImplementation(stringBuilder.value, LogType::Error, errorCode);
	}

	static string GetLogAsString(const DebugLog& log) noexcept;

protected:
	std::ofstream* outputFile;

	virtual DebugLog LogImplementation(const string& message, const LogType type = Message, const LogID id = Undefined);
};