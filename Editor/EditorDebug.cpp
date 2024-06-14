#include "EditorDebug.h"

#include "ConsoleGUI.h"

Debug::DebugLog EditorDebug::LogImplementation(const string& message, const LogType type, const LogID id)
{
   DebugLog log = Debug::LogImplementation(message, type, id);

   if (type != Debug::Subtle) ConsoleGUI::logs.push_back(log);

   return log;
}
