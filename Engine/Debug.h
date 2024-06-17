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

#include "LogID.h"

class Debug;
extern Debug* debug;

template<typename T> concept MsgType = requires(T t) { { ValueAsString(t) } -> std::same_as<std::string>; };

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



	template<MsgType Req, MsgType... Opt>
	static DebugLog Log(Req message, Opt... messageContinued)
	{
		return debug->LogImplementation(StringBuilder(message, messageContinued...).value, LogType::Message);
	}
	template<MsgType Req, MsgType... Opt>
	static DebugLog LogSubtle(Req message, Opt... messageContinued)
	{
		return debug->LogImplementation(StringBuilder(message, messageContinued...).value, LogType::Subtle);
	}

	template<typename Req, MsgType... Opt>
	static DebugLog LogWarning(Req messageOrWarningCode, Opt... messageContinued)
	{
		if constexpr (std::is_same_v<Req, LogID>)
		{
			return debug->LogImplementation(StringBuilder(messageContinued...).value, LogType::Warning, messageOrWarningCode);
		}
		else
		{
			return debug->LogImplementation(StringBuilder(messageOrWarningCode, messageContinued...).value, LogType::Warning);
		}
	}

	template<typename Req, MsgType... Opt>
	static DebugLog LogError(Req messageOrErrorCode, Opt... messageContinued)
	{
		if constexpr (std::is_same_v<Req, LogID>)
		{
			if constexpr (sizeof...(Opt) > 0)
			{
				return debug->LogImplementation(StringBuilder(messageContinued...).value, LogType::Error, messageOrErrorCode);
			}
			else
			{
				return debug->LogImplementation(std::string(), LogType::Error, messageOrErrorCode);
			}
		}
		else
		{
			return debug->LogImplementation(StringBuilder(messageOrErrorCode, messageContinued...).value, LogType::Error);
		}
		
	}

	static string GetLogAsString(const DebugLog& log) noexcept;

protected:
	std::ofstream* outputFile;

	virtual DebugLog LogImplementation(const string& message, const LogType type = Message, const LogID id = LogID::Undefined);
};
