#include "TileEditor.h"

vector<TileData>* TileEditor::selectedType = &HexTile::trees;
uint TileEditor::selectedVariant;

void TileEditor::Draw(const char* const name, bool& open) noexcept
{
	ImGui::Begin(name, &open);

	if (ImGui::CollapsingHeader("Radial Sprites"))
	{
		ImGui::Indent();
		if (ImGui::CollapsingHeader("Tree"))
		{
			ImGui::PushID("Tree");
			DrawSpriteInput("Regular", HexTile::treeRadialSprites[1]);
			DrawSpriteInput("Hovered", HexTile::treeRadialSprites[0]);
			DrawSpriteInput("Locked ", HexTile::treeRadialSprites[2]);
			ImGui::PopID();
			GUI::Spacing(3);
		}
		if (ImGui::CollapsingHeader("Flower"))
		{
			ImGui::PushID("Flower");
			DrawSpriteInput("Regular", HexTile::flowerRadialSprites[1]);
			DrawSpriteInput("Hovered", HexTile::flowerRadialSprites[0]);
			DrawSpriteInput("Locked ", HexTile::flowerRadialSprites[2]);
			ImGui::PopID();
			GUI::Spacing(3);
		}
		if (ImGui::CollapsingHeader("Water"))
		{
			ImGui::PushID("Water");
			DrawSpriteInput("Regular", HexTile::waterRadialSprites[1]);
			DrawSpriteInput("Hovered", HexTile::waterRadialSprites[0]);
			DrawSpriteInput("Locked ", HexTile::waterRadialSprites[2]);
			ImGui::PopID();
			GUI::Spacing(3);
		}
		if (ImGui::CollapsingHeader("Land"))
		{
			ImGui::PushID("Land");
			DrawSpriteInput("Regular", HexTile::landRadialSprites[1]);
			DrawSpriteInput("Hovered", HexTile::landRadialSprites[0]);
			DrawSpriteInput("Locked ", HexTile::landRadialSprites[2]);
			ImGui::PopID();
			GUI::Spacing(3);
		}
		ImGui::Unindent();
	}

	GUI::Spacing(3);

	if (ImGui::CollapsingHeader("Defaults"))
	{
		GUI::Spacing(3);

		DrawPrefabInput("Default", HexTile::defaultTilePath);
		ImGui::TextWrapped("Left behind when moving a habitat");
		GUI::Spacing(3);
		DrawPrefabInput("Empty  ", HexTile::emptyTilePath);
		ImGui::TextWrapped("Empty tile that can be placed on");

		GUI::Spacing(3);
	}

	GUI::Spacing(3);

	if (ImGui::CollapsingHeader("Tile Variants"))
	{
		GUI::Spacing(3);

		ImGui::BeginTabBar("Types");
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

		GUI::Spacing(3);

		if (selectedType != nullptr && selectedVariant < selectedType->size())
		{
			GUI::Spacing(3);
			ImGui::SeparatorText("Selected Variant");

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
	if (ImGui::Button(StringBuilder("Load##", name).CStr()))
	{
		using namespace FileDialogue;

		string path = GetLoadPath(PathDetails("Tile Prefab", "Assets\\", { "*.prefab" }), LimitToAssetFolder::True);
		if (path.size() > 0)
		{
			prefabFilepath = path;
		}
	}
}

void TileEditor::DrawSpriteInput(const char* const name, string& spriteFilepath)
{
	ImGui::BeginDisabled();
	ImGui::InputText(name, &spriteFilepath, ImGuiInputTextFlags_ReadOnly);
	ImGui::EndDisabled();

	ImGui::SameLine();
	if (ImGui::Button(StringBuilder("Load##", name).CStr()))
	{
		using namespace FileDialogue;

		string path = GetLoadPath(PathDetails("Radial Sprite", "Assets\\", { "*.png", "*.tga", "*.jpg" }), LimitToAssetFolder::True);
		if (path.size() > 0)
		{
			spriteFilepath = path;
		}
	}
}
