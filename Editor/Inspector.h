#pragma once

#include "GameObjectGUI.h"

class Inspector
{
public:
	Inspector() = default;
	~Inspector();
	Inspector(const Inspector&) = delete;
	Inspector& operator=(const Inspector&) = delete;

	void SetTarget(GameObject* target);
	const GameObjectGUI* GetTargetGUI();
	GameObject* GetTarget();

	void Draw();

private:
	GameObjectGUI* targetGUI;
};

extern Inspector* inspector;