#pragma once

#include "ImGuiIncludes.h"
#include "GameObjectGUI.h"

#include "GameObject3D.h"

class GameObject3DGUI : public GameObjectGUI
{
public:
	using GameObjectGUI::GameObjectGUI;

	virtual void Draw() override;

protected:
	static void DrawGameObject3DGUI(GameObject3D* gameObject3D);
};