#include "Inspector.h"

Inspector* inspector = new Inspector();

#include "GameObject3DGUI.h"
#include "CameraGUI.h"
#include "MeshRendererGUI.h"
#include "LightObjectGUI.h"
#include "ParticleEmitterGUI.h"
#include "LinkedHexGridGUI.h"
#include "HexTileGUI.h"
#include "HabitatGUI.h"

#include "GeneralMacros.h"

void Inspector::SetTarget(GameObject* target)
{
	if (targetGUI != nullptr)
	{
		targetGUI->target->selected = false;
		delete targetGUI;
		targetGUI = nullptr;
	};

	if (target == nullptr) return;

	switch (target->GetClassID())
	{
	case GameObject::classID: targetGUI = new GameObjectGUI(target); break;
	case GameObject3D::classID: targetGUI = new GameObject3DGUI(target); break;
	case Camera::classID: targetGUI = new CameraGUI(target); break;
	case MeshRenderer::classID: targetGUI = new MeshRendererGUI(target); break;
	case LightObject::classID: targetGUI = new LightObjectGUI(target); break;
	case ParticleEmitter::classID: targetGUI = new ParticleEmitterGUI(target); break;
	case LinkedHexGrid::classID: targetGUI = new LinkedHexGridGUI(target); break;
	case HexTileObject::classID: targetGUI = new HexTileGUI(target); break;
	case HabitatObject::classID: targetGUI = new HabitatGUI(target); break;
	default:
		targetGUI = new GameObjectGUI(target);
		Debug::LogError(LogID::ERR151,
			target->GetClassName(), " "
			"was not accounted for when selecting inspector display.",
			"Editable fields will be limited."
			locationinfo
		);
		break;
	}

	target->selected = true;
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
