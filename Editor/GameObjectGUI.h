#pragma once

#include "InspectableObjectGUI.h"

#include "GameObject.h"

class GameObjectGUI : public InspectableObjectGUI
{
public:
	using InspectableObjectGUI::InspectableObjectGUI;

	virtual void Draw() override;

protected:
	static void DrawGameObjectGUI(GameObject* gameObject);
};