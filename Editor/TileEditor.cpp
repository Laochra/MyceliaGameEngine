#include "TileEditor.h"

uint TileEditor::selectionIndices[2] = { 0U, 0U };

vector<vector<TileData>> TileEditor::variants = {
	vector<TileData>(),
	vector<TileData>(),
	vector<TileData>(),
	vector<TileData>()
};


void TileEditor::Draw(const char* const name, bool& open) noexcept
{
	ImGui::Begin(name, &open);

	ImGui::BeginTabBar("Types");
	if (ImGui::BeginTabItem("Tree"))
	{
		DrawType(Tree);
		ImGui::EndTabItem();
	};
	if (ImGui::BeginTabItem("Flower"))
	{
		DrawType(Flower);
		ImGui::EndTabItem();
	};
	if (ImGui::BeginTabItem("Water"))
	{
		DrawType(Water);
		ImGui::EndTabItem();
	};
	if (ImGui::BeginTabItem("Land"))
	{
		DrawType(Land);
		ImGui::EndTabItem();
	};
	ImGui::EndTabBar();

	if (selectionIndices[1] < variants[selectionIndices[0]].size())
	{
		GUI::Spacing(3);
		ImGui::Separator();
		GUI::Spacing(3);

		ImGui::InputText("Name", &SelectedTileData().name);
		ImGui::SameLine();
		GUI::HSpacing(3);
		ImGui::SameLine();
		if (ImGui::Button("Delete"))
		{
			variants[selectionIndices[0]].erase(variants[selectionIndices[0]].begin() + selectionIndices[1]);
			selectionIndices[1] = 0U;
			if (variants[selectionIndices[0]].size() == 0)
			{
				ImGui::End();
				return;
			}
		}

		GUI::Spacing(3);

		ImGui::Text("Tile Prefabs by Density");
		DrawPrefabInput("Low  ", SelectedTileData().prefabFilepaths[0]);
		DrawPrefabInput("Mid  ", SelectedTileData().prefabFilepaths[1]);
		DrawPrefabInput("High ", SelectedTileData().prefabFilepaths[2]);
	}

	ImGui::End();
}

void TileEditor::DrawType(TileType type) noexcept
{
	vector<TileData>& variantsOfType = variants[type];
	
	if (ImGui::Button("Add"))
	{
		variantsOfType.push_back(TileData("New Variant"));
	}

	for (uint i = 0; i < (uint)variantsOfType.size(); i++)
	{
		bool isSelected = type == selectionIndices[0] && i == selectionIndices[1];

		if (ImGui::Selectable((" " + variantsOfType[i].name + "##" + std::to_string(i)).c_str(), isSelected))
		{
			selectionIndices[0] = type;
			selectionIndices[1] = i;
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

TileData& TileEditor::SelectedTileData() noexcept
{
	return variants[selectionIndices[0]][selectionIndices[1]];
}
