#pragma once

#include "InspectableObjectGUI.h"
#include "UISprite.h"

class UISpriteGUI : public InspectableObjectGUI
{
public:
	using InspectableObjectGUI::InspectableObjectGUI;

	virtual void Draw() override;

protected:
	static void DrawUISpriteGUI(UISprite* uiSprite);
};
