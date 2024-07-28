#include "HexTileGUI.h"
#include "LinkedHexGrid.h"

#include "GUI.h"

void HexTileGUI::Draw()
{
	DrawHexTileGUI((HexTile*)target);
}

void HexTileGUI::DrawHexTileGUI(HexTile* hexTile)
{
	if (hexTile == nullptr || hexTile == GameObject::Destroyed) return;

	MeshRendererGUI::DrawMeshRendererGUI(hexTile);

	std::string idStr = GUI::GenerateID(HexTile::className, hexTile);
	const char* id = idStr.c_str();

	if (ImGui::CollapsingHeader(id, ImGuiTreeNodeFlags_DefaultOpen))
	{
		const char* hexTypeName;
		switch (hexTile->type)
		{
		case HexType::Available:	hexTypeName = "Available";	break;
		case HexType::Grass:			hexTypeName = "Grass";		break;
		case HexType::Water:			hexTypeName = "Water";		break;
		default:							hexTypeName = "Invalid";	break;
		}
		if (ImGui::BeginCombo("HexType", hexTypeName))
		{
			if (ImGui::Selectable("Available", hexTile->type == HexType::Available))
			{
				LinkedHexGrid* grid = dynamic_cast<LinkedHexGrid*>(hexTile->GetParent());

				if (grid != nullptr) grid->UpdateTile(hexTile, HexType::Available);
				else Debug::LogError("HexTiles should be children of a LinkedHexGrid", locationinfo);
			}
			if (ImGui::Selectable("Grass", hexTile->type == HexType::Grass))
			{
				LinkedHexGrid* grid = dynamic_cast<LinkedHexGrid*>(hexTile->GetParent());

				if (grid != nullptr) grid->UpdateTile(hexTile, HexType::Grass);
				else Debug::LogError("HexTiles should be children of a LinkedHexGrid", locationinfo);
			}
			if (ImGui::Selectable("Water", hexTile->type == HexType::Water))
			{
				LinkedHexGrid* grid = dynamic_cast<LinkedHexGrid*>(hexTile->GetParent());

				if (grid != nullptr) grid->UpdateTile(hexTile, HexType::Water);
				else Debug::LogError("HexTiles should be children of a LinkedHexGrid", locationinfo);
			}
			ImGui::EndCombo();
		}

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
