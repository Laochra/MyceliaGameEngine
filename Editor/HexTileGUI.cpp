#include "HexTileGUI.h"

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
				hexTile->type = HexType::Available;
				//hexTile->UpdateFrom(HexTile::availablePrefab);
			}
			if (ImGui::Selectable("Grass", hexTile->type == HexType::Grass))
			{
				hexTile->type = HexType::Grass;
				//hexTile->UpdateFrom(HexTile::grassPrefab);
			}
			if (ImGui::Selectable("Water", hexTile->type == HexType::Water))
			{
				hexTile->type = HexType::Water;
				//hexTile->UpdateFrom(HexTile::waterPrefab);
			}
			ImGui::EndCombo();
		}

		GUI::Spacing(3);
	}
}
