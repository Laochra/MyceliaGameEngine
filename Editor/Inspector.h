#pragma once

#include "InspectableObject.h"
#include "GameObjectGUI.h"

class Inspector
{
public:
	Inspector() = default;
	~Inspector();
	Inspector(const Inspector&) = delete;
	Inspector& operator=(const Inspector&) = delete;

	void SetTarget(InspectableObject* target);
	const GameObjectGUI* GetTargetGUI();
	InspectableObject* GetTarget();

	static void Draw(const char* const name, bool& open);

private:
	GameObjectGUI* targetGUI = nullptr;
};

extern Inspector* inspector;