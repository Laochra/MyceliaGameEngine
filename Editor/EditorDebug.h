#pragma once

#include "Debug.h"

class EditorDebug : public Debug
{
protected:
	DebugLog LogImplementation(const string& message, const LogType type, const LogID id) override;
};
