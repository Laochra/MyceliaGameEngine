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

	if (target == nullptr) return;

	switch (target->GetClassID())
	{
	case GameObject::classID: targetGUI = new GameObjectGUI(target); break;
	case GameObject3D::classID: targetGUI = new GameObject3DGUI(target); break;
	case MeshRenderer::classID: targetGUI = new MeshRendererGUI(target); break;
	case LightObject::classID: targetGUI = new LightObjectGUI(target); break;
	case ParticleEmitter::classID: targetGUI = new ParticleEmitterGUI(target); break;
	default:
		targetGUI = new GameObjectGUI(target);
		Debug::Log(
			Debug::ERR(
				target->GetClassName(), " "
				"was not accounted for when selecting inspector display.",
				"Editable fields will be limited."
				locationinfo
			),
			Debug::ERR151
		);
		break;
	}
}

const GameObjectGUI* Inspector::GetTargetGUI()
{
	return targetGUI;
}

GameObject* Inspector::GetTarget()
{
	if (targetGUI == nullptr) return nullptr;
	return targetGUI->target;
}

void Inspector::Draw(const char* const name, bool& open)
{
	ImGui::Begin(name, &open);

	if (inspector->targetGUI != nullptr)
	{
		if (inspector->targetGUI->target == nullptr || inspector->targetGUI->target != GameObject::Destroyed)
		{
			inspector->targetGUI->Draw();
			inspector->targetGUI->target->DrawDebug();
		}
		else
		{
			del(inspector->targetGUI);
		}
	}

	ImGui::End();
}

Inspector::~Inspector()
{
	del(targetGUI);
}
