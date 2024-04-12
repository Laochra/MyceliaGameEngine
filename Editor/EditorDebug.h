#pragma once

#include "Debug.h"

class EditorDebug : public Debug
{
public:
	DebugLog Log(StringParams message, LogType type = Message, LogID id = Undefined) override;
	DebugLog Log(LogType type, LogID id = Undefined) override;
};