#include "Inspector.h"

Inspector* inspector = new Inspector();

#include "GameObject3DGUI.h"
#include "MeshRendererGUI.h"
#include "LightObjectGUI.h"

#include "GeneralMacros.h"

void Inspector::SetTarget(GameObject* target)
{
	del(targetGUI);

	switch (target->GetClassID())
	{
	case GameObject::classID:	 targetGUI = new GameObjectGUI(target);	break;
	case GameObject3D::classID: targetGUI = new GameObject3DGUI(target); break;
	case MeshRenderer::classID: targetGUI = new MeshRendererGUI(target); break;
	case LightObject::classID:  targetGUI = new LightObjectGUI(target);  break;
	}
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
	del(targetGUI);
}
