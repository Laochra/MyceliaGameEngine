#include "Inspector.h"

Inspector* inspector = new Inspector();

#include "GameObject3DGUI.h"
#include "CameraGUI.h"
#include "MeshRendererGUI.h"
#include "LightObjectGUI.h"
#include "ParticleEmitterGUI.h"
#include "LinkedHexGridGUI.h"

#include "GeneralMacros.h"

void Inspector::SetTarget(InspectableObject* target)
{
	if (targetGUI != nullptr)
	{
		targetGUI->target->selected = false;
		delete targetGUI;
		targetGUI = nullptr;
	};

	if (target == nullptr) return;

	GameObject* gameObject = dynamic_cast<GameObject*>(target);
	if (gameObject != nullptr)
	{
		switch (gameObject->GetClassID())
		{
		case GameObject::classID: targetGUI = new GameObjectGUI(gameObject); break;
		case GameObject3D::classID: targetGUI = new GameObject3DGUI(gameObject); break;
		case Camera::classID: targetGUI = new CameraGUI(gameObject); break;
		case MeshRenderer::classID: targetGUI = new MeshRendererGUI(gameObject); break;
		case LightObject::classID: targetGUI = new LightObjectGUI(gameObject); break;
		case ParticleEmitter::classID: targetGUI = new ParticleEmitterGUI(gameObject); break;
		case LinkedHexGrid::classID: targetGUI = new LinkedHexGridGUI(gameObject); break;
		default:
			targetGUI = new GameObjectGUI(gameObject);
			Debug::LogError(LogID::ERR151,
				gameObject->GetClassName(), " "
				"was not accounted for when selecting inspector display.",
				"Editable fields will be limited."
				locationinfo
			);
			break;
		}
	}

	target->selected = true;
}

const GameObjectGUI* Inspector::GetTargetGUI()
{
	return targetGUI;
}

InspectableObject* Inspector::GetTarget()
{
	if (targetGUI == nullptr) return nullptr;
	return targetGUI->target;
}

void Inspector::Draw(const char* const name, bool& open)
{
	ImGui::Begin(name, &open);

	if (inspector->targetGUI != nullptr)
	{
		GameObject* gameObject = dynamic_cast<GameObject*>(inspector->targetGUI->target);

		if (inspector->targetGUI->target != nullptr)
		{
			inspector->targetGUI->Draw();
			if (gameObject != nullptr && gameObject != GameObject::Destroyed)
			{
				gameObject->DrawDebug();
			}
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
