#include "EditorDebug.h"

#include "ConsoleGUI.h"

Debug::DebugLog EditorDebug::Log(const StringParams& message, const LogType type, const LogID id)
{
   DebugLog log = Debug::Log(message, type, id);

   if (type != Debug::Subtle) ConsoleGUI::logs.push_back(log);

   return log;
}
Debug::DebugLog EditorDebug::Log(const string& message, const LogType type, const LogID id) { return Log({ message }, type, id); }
Debug::DebugLog EditorDebug::Log(const LogType type, const LogID id) { return Log(StringParams(), type, id); }
