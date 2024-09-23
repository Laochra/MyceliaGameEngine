#pragma once

#include "ImGuiIncludes.h"

#include <sstream>

#include "InspectableObject.h"

class InspectableObjectGUI
{
public:
	InspectableObject* target;
	virtual void Draw() = 0;
	InspectableObjectGUI(InspectableObject* targetInit);
};
