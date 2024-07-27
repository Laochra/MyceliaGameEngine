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

#include "AppInfo.h"

template<typename T> concept MsgType =
	requires(T t) { std::same_as<decltype(ValueAsString(t)), std::string>; };
template<typename T> concept MsgTypeOrLogID =
	requires(T t) { std::same_as<decltype(ValueAsString(t)), std::string>; } ||
	std::same_as<T, LogID>;
	 
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



	/// <summary>
	/// <para>Logs a message.</para>
	/// <para>As many parameters as is required can be used.</para>
	/// <param name='message/messageContinued:'>All primitive types and other types with ToString(x) overridden can be used and will be converted to strings nicely.</param>
	/// </summary>
	template<MsgType Req, MsgType... Opt>
	static DebugLog Log(Req message, Opt... messageContinued)
	{
		return AppInfo::debug->LogImplementation(StringBuilder(message, messageContinued...).value, LogType::Message);
	}

	/// <summary>
	/// <para>Logs a message subtly (certain contexts won't show subtle logs to avoid spam, but this will always show in the log file).</para>
	/// <para>As many parameters as is required can be used.</para>
	/// <param name='message/messageContinued:'>All primitive types and other types with ToString(x) overridden can be used and will be converted to strings nicely.</param>
	/// </summary>
	template<MsgType Req, MsgType... Opt>
	static DebugLog LogSubtle(Req message, Opt... messageContinued)
	{
		return AppInfo::debug->LogImplementation(StringBuilder(message, messageContinued...).value, LogType::Subtle);
	}

	/// <summary>
	/// <para>Logs a warning which appears yellow.</para>
	/// <para>As many parameters as is required can be used.</para>
	/// <para>The first parameter can optionally be a LogID which inserts a generic message based on the LogID.</para>
	/// <para>All primitive types and other types with ToString(x) overridden can be used and will be converted to strings nicely.</para>
	/// </summary>
	template<MsgTypeOrLogID Req, MsgType... Opt>
	static DebugLog LogWarning(Req messageOrWarningCode, Opt... messageContinued)
	{
		if constexpr (std::is_same_v<Req, LogID>)
		{
			return AppInfo::debug->LogImplementation(StringBuilder(messageContinued...).value, LogType::Warning, messageOrWarningCode);
		}
		else
		{
			return AppInfo::debug->LogImplementation(StringBuilder(messageOrWarningCode, messageContinued...).value, LogType::Warning);
		}
	}

	
	/// <summary>
	/// <para>Logs an error which appears red.</para>
	/// <para>As many parameters as is required can be used.</para>
	/// <para>The first parameter can optionally be a LogID which inserts a generic message based on the LogID.</para>
	/// <para>All primitive types and other types with ToString(x) overridden can be used and will be converted to strings nicely.</para>
	/// </summary>
	template<MsgTypeOrLogID Req, MsgType... Opt>
	static DebugLog LogError(Req messageOrErrorCode, Opt... messageContinued)
	{
		if constexpr (std::is_same_v<Req, LogID>)
		{
			if constexpr (sizeof...(Opt) > 0)
			{
				return AppInfo::debug->LogImplementation(StringBuilder(messageContinued...).value, LogType::Error, messageOrErrorCode);
			}
			else
			{
				return AppInfo::debug->LogImplementation(std::string(), LogType::Error, messageOrErrorCode);
			}
		}
		else
		{
			return AppInfo::debug->LogImplementation(StringBuilder(messageOrErrorCode, messageContinued...).value, LogType::Error);
		}
		
	}

	static string GetLogAsString(const DebugLog& log) noexcept;

protected:
	std::ofstream* outputFile;

	virtual DebugLog LogImplementation(const string& message, const LogType type = Message, const LogID id = LogID::Undefined);
};
