#include "EditorDebug.h"

#include "ConsoleGUI.h"

Debug::DebugLog EditorDebug::Log(StringParams message, LogType type, LogID id)
{
   DebugLog log = Debug::Log(message, type, id);

   ConsoleGUI::logs.push_back(log);

   return log;
}

Debug::DebugLog EditorDebug::Log(LogType type, LogID id)
{
   return Log(StringParams(), type, id);
}
