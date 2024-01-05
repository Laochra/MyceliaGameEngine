#pragma once

#include "ImGuiIncludes.h"
#include "GUI.h"

#include <sstream>

#include "GameObject.h"

namespace GameObjectGUI
{
	inline void Draw(GameObject* gameObject);
}

void GameObjectGUI::Draw(GameObject* gameObject)
{
	std::string idStr = GUI::GenerateID("GameObject", gameObject);
	const char* id = idStr.c_str();

	if (ImGui::CollapsingHeader(id, ImGuiTreeNodeFlags_DefaultOpen))
	{
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