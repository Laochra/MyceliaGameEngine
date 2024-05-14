#include "Inspector.h"

Inspector* inspector = new Inspector();

#include "GameObject3DGUI.h"
#include "MeshRendererGUI.h"
#include "LightObjectGUI.h"
#include "ParticleEmitterGUI.h"

#include "GeneralMacros.h"

void Inspector::SetTarget(GameObject* target)
{
	del(targetGUI);

	const char* targetClassName = target->ClassName();

	#define SetGuiIfEqual(className) if (targetClassName == stringify(className)) { targetGUI = new className##GUI(target); }

	SetGuiIfEqual(GameObject)
	else SetGuiIfEqual(GameObject3D)
	else SetGuiIfEqual(MeshRenderer)
	else SetGuiIfEqual(LightObject)
	else SetGuiIfEqual(ParticleEmitter)

	#undef SetGuiIfEqual
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
	targetGUI->target->DrawDebug();
}

Inspector::~Inspector()
{
	del(targetGUI);
}
