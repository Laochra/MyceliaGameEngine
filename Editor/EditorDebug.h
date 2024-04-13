#pragma once

#include "Debug.h"

class EditorDebug : public Debug
{
public:
	DebugLog Log(const StringParams& message, const LogType type = Message, const LogID id = Undefined) override;
	DebugLog Log(const string& message, const LogType type, const LogID id) override;
	DebugLog Log(const LogType type, const LogID id = Undefined) override;
};
