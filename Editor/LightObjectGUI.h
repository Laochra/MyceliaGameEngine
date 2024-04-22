#pragma once

#include "GameObject3DGUI.h"

#include "LightObject.h"

class LightObjectGUI : public GameObject3DGUI
{
public:
	using GameObject3DGUI::GameObject3DGUI;

	virtual void Draw() override;

protected:
	static void DrawLightObjectGUI(LightObject* lightObject);
};