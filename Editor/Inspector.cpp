#include "Inspector.h"

Inspector* inspector = new Inspector();

#include "GameObject3DGUI.h"
#include "MeshRendererGUI.h"

void Inspector::SetTarget(GameObject* target)
{
	if (targetGUI != nullptr) { delete targetGUI; }

	const char* targetClassName = target->ClassName();

	if (targetClassName == "GameObject")			{ targetGUI = new GameObjectGUI(target); }
	else if (targetClassName == "GameObject3D")	{ targetGUI = new GameObject3DGUI(target); }
	else if (targetClassName == "MeshRenderer")	{ targetGUI = new MeshRendererGUI(target); }
}

const GameObjectGUI* Inspector::GetTargetGUI()
{
	return targetGUI;
}

GameObject* Inspector::GetTarget()
{
	return targetGUI->target;
}

void Inspector::Draw()
{
	if (targetGUI == nullptr) return;

	targetGUI->Draw();
}

Inspector::~Inspector()
{
	if (targetGUI != nullptr) { delete targetGUI; }
}
