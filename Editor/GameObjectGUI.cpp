#include "GameObjectGUI.h"

#include "GUI.h"

#include "StringIncludes.h"

void GameObjectGUI::Draw()
{
	DrawGameObjectGUI((GameObject*)target);
}

void GameObjectGUI::DrawGameObjectGUI(GameObject* gameObject)
{
	if (gameObject == nullptr || gameObject == GameObject::Destroyed) return;

	std::string idStr = GUI::GenerateID(GameObject::className, gameObject);
	const char* id = idStr.c_str();

	if (ImGui::CollapsingHeader(id, ImGuiTreeNodeFlags_DefaultOpen))
	{
		std::string tempName(gameObject->GetName());
		if (ImGui::InputText(" ", &tempName))
		{
			gameObject->SetName(tempName.c_str());
		}
		ImGui::SameLine();
		bool active = gameObject == GameObject::Active;
		ImGui::Checkbox("Active", &active);
		if (active != (gameObject == GameObject::Active))
		{
			gameObject->SetState(active ? GameObject::Active : GameObject::Inactive);
		}

		std::ostringstream stream;
		stream << "GUID: " << gameObject->GetGUID();
		ImGui::Text(stream.str().c_str());

		ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
	}
}