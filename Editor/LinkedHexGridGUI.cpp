#include "LinkedHexGridGUI.h"

#include "GUI.h"

void LinkedHexGridGUI::Draw()
{
	DrawLinkedHexGridGUI((LinkedHexGrid*)target);
}

void LinkedHexGridGUI::DrawLinkedHexGridGUI(LinkedHexGrid* linkedHexGrid)
{
	if (linkedHexGrid == nullptr || linkedHexGrid == GameObject::Destroyed) return;

	GameObject3DGUI::DrawGameObject3DGUI(linkedHexGrid);

	std::string idStr = GUI::GenerateID(LinkedHexGrid::className, linkedHexGrid);
	const char* id = idStr.c_str();

	if (ImGui::CollapsingHeader(id, ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("---");
		GUI::Spacing(3);
	}
}
