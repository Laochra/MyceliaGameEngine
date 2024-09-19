#include "HexTileGUI.h"
#include "LinkedHexGrid.h"

#include "GUI.h"

void HexTileGUI::Draw()
{
	DrawHexTileGUI((HexTileObject*)target);
}

void HexTileGUI::DrawHexTileGUI(HexTileObject* hexTile)
{
	if (hexTile == nullptr || hexTile == GameObject::Destroyed) return;

	MeshRendererGUI::DrawMeshRendererGUI(hexTile);

	std::string idStr = GUI::GenerateID(HexTileObject::className, hexTile);
	const char* id = idStr.c_str();

	if (ImGui::CollapsingHeader(id, ImGuiTreeNodeFlags_DefaultOpen))
	{
		int adjacentHexes = 0;
		for (int i = 0; i < 6; i++)
		{
			if (hexTile->adjacent[i] != nullptr) adjacentHexes++;
		}
		glm::ivec2 hexPos(hexTile->GetHexPos());
		ImGui::DragInt2("HexPos", (int*)&hexPos);
		ImGui::Text(StringBuilder("Adjacent Hexes: ", adjacentHexes).CStr());

		GUI::Spacing(3);
	}
}
