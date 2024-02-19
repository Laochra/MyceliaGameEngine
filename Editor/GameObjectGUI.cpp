#include "GameObjectGUI.h"

#include "GUI.h"

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
	std::string idStr = GUI::GenerateID("GameObject", gameObject);
	const char* id = idStr.c_str();

	if (ImGui::CollapsingHeader(id, ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::InputText(" ", &gameObject->name));
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