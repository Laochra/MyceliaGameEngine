#pragma once

#include "ImGuiIncludes.h"

#include <sstream>

#include "GameObject.h"

class GameObjectGUI
{
public:
	GameObject* target;
	virtual void Draw();
	
	GameObjectGUI(GameObject* targetInit);

protected:
	static void DrawGameObjectGUI(GameObject* gameObject);
};