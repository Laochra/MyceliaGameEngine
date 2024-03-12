#include "GameObjectGUI.h"

#include "GUI.h"

#include "StringIncludes.h"

void GameObjectGUI::Draw()
{
	DrawGameObjectGUI(target);
}

GameObjectGUI::GameObjectGUI(GameObject* targetInit)
{
	target = targetInit;
}

void GameObjectGUI::DrawGameObjectGUI(GameObject* gameObject)
{
	if (gameObject == nullptr || *gameObject == GameObject::Destroyed) return;

	std::string idStr = GUI::GenerateID("GameObject", gameObject);
	const char* id = idStr.c_str();

	if (ImGui::CollapsingHeader(id, ImGuiTreeNodeFlags_DefaultOpen))
	{
		std::string tempName(gameObject->name);
		if (ImGui::InputText(" ", &tempName))
		{
			delete gameObject->name;
			AssignStringToCString(gameObject->name, tempName);
		}
		ImGui::SameLine();
		bool active = gameObject->GetState() == GameObject::Active;
		ImGui::Checkbox("Active", &active);
		if (active != (gameObject->GetState() == GameObject::Active))
		{
			gameObject->SetState(active ? GameObject::Active : GameObject::Inactive);
		}

		std::ostringstream stream;
		stream << "GUID: " << gameObject->GetGUID();
		ImGui::Text(stream.str().c_str());

		ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
	}
}