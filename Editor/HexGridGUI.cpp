#include "HexGridGUI.h"

#include "GUI.h"

void HexGridGUI::Draw()
{
	DrawHexGridGUI((HexGrid*)target);
}

void HexGridGUI::DrawHexGridGUI(HexGrid* hexGrid)
{
	if (hexGrid == nullptr || hexGrid == GameObject::Destroyed) return;

	GameObject3DGUI::DrawGameObject3DGUI(hexGrid);

	std::string idStr = GUI::GenerateID(HexGrid::className, hexGrid);
	const char* id = idStr.c_str();

	if (ImGui::CollapsingHeader(id, ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("---");
		GUI::Spacing(3);
	}
}
