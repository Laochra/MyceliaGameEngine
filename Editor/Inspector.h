#pragma once

#include "InspectableObjectGUI.h"

class Inspector
{
public:
	Inspector() = default;
	~Inspector();
	Inspector(const Inspector&) = delete;
	Inspector& operator=(const Inspector&) = delete;

	void SetTarget(InspectableObject* target);
	const InspectableObjectGUI* GetTargetGUI();
	InspectableObject* GetTarget();

	static void Draw(const char* const name, bool& open);

private:
	InspectableObjectGUI* targetGUI = nullptr;
};

extern Inspector* inspector;