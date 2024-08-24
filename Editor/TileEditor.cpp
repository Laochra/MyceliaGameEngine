#include "TileEditor.h"

vector<TileData>* TileEditor::selectedType = &HexTile::trees;
uint TileEditor::selectedVariant;

void TileEditor::Draw(const char* const name, bool& open) noexcept
{
	ImGui::Begin(name, &open);

	ImGui::BeginTabBar("Types");
	if (ImGui::BeginTabItem("Defaults"))
	{
		selectedType = nullptr;

		GUI::Spacing(3);
		DrawPrefabInput("Default", HexTile::defaultTilePath);
		ImGui::TextWrapped("Left behind when moving a habitat");
		GUI::Spacing(3);
		DrawPrefabInput("Empty  ", HexTile::emptyTilePath);
		ImGui::TextWrapped("Empty tile that can be placed on");

		ImGui::EndTabItem();
	}
	if (ImGui::BeginTabItem("Tree"))
	{
		DrawType(HexTile::trees);
		ImGui::EndTabItem();
	}
	if (ImGui::BeginTabItem("Flower"))
	{
		DrawType(HexTile::flowers);
		ImGui::EndTabItem();
	}
	if (ImGui::BeginTabItem("Water"))
	{
		DrawType(HexTile::waters);
		ImGui::EndTabItem();
	}
	if (ImGui::BeginTabItem("Land"))
	{
		DrawType(HexTile::lands);
		ImGui::EndTabItem();
	}
	ImGui::EndTabBar();

	if (selectedType != nullptr && selectedVariant < selectedType->size())
	{
		GUI::Spacing(3);
		ImGui::Separator();
		GUI::Spacing(3);

		ImGui::InputText("Name", &(*selectedType)[selectedVariant].name);
		ImGui::SameLine();
		GUI::HSpacing(3);
		ImGui::SameLine();
		if (ImGui::Button("Delete"))
		{
			selectedType->erase(selectedType->begin() + selectedVariant);
			selectedVariant = 0U;
			if (selectedType->size() == 0)
			{
				ImGui::End();
				return;
			}
		}

		GUI::Spacing(3);

		ImGui::Text("Tile Prefabs by Density");
		DrawPrefabInput("Low  ", (*selectedType)[selectedVariant].prefabFilepaths[0]);
		DrawPrefabInput("Mid  ", (*selectedType)[selectedVariant].prefabFilepaths[1]);
		DrawPrefabInput("High ", (*selectedType)[selectedVariant].prefabFilepaths[2]);
	}

	ImGui::End();
}

void TileEditor::DrawType(vector<TileData>& type) noexcept
{	
	if (&type != selectedType)
	{
		selectedType = &type;
		selectedVariant = 0U;
	}

	if (ImGui::Button("Add"))
	{
		type.push_back(TileData("New Variant"));
	}

	for (uint i = 0; i < (uint)type.size(); i++)
	{
		bool isSelected = i == selectedVariant;

		if (ImGui::Selectable((" " + type[i].name + "##" + std::to_string(i)).c_str(), isSelected))
		{
			selectedVariant = i;
		}
	}
}

void TileEditor::DrawPrefabInput(const char* const name, string& prefabFilepath)
{
	ImGui::BeginDisabled();
	ImGui::InputText(name, &prefabFilepath, ImGuiInputTextFlags_ReadOnly);
	ImGui::EndDisabled();

	ImGui::SameLine();
	if (ImGui::Button("Load"))
	{

	}
}
