#include "TileEditor.h"

#include "Habitat.h"

vector<TileData>* TileEditor::selectedType = &HexTile::trees;
uint TileEditor::selectedVariant;

void TileEditor::Draw(const char* const name, bool& open) noexcept
{
	ImGui::Begin(name, &open);

	GUI::Spacing(3);
	if (ImGui::Button("Save Changes"))
	{
		const char* const tileDataPath = "Assets\\Tiles\\TileData.json";

		json tileData;
		HexTile::SaveTileDataTo(tileData);

		ofstream output(tileDataPath);
		output << std::setw(2) << tileData;
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

		ImGui::BeginTabBar("Types##Tile Variants");
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

		GUI::Spacing(3);
	}

	GUI::Spacing(3);

	if (ImGui::CollapsingHeader("Habitats"))
	{
		GUI::Spacing(3);

		ImGui::BeginTabBar("Habitats##Habitats");
		for (uint i = 0; i < (uint)Habitat::habitats.size(); i++)
		{
			HabitatData& habitat = Habitat::habitats[i];

			if (ImGui::BeginTabItem(StringBuilder(habitat.name, "###", i).CStr()))
			{
				ImGui::InputText("Name ", &habitat.name);

				ImGui::SameLine();
				if ((ImGui::Button("Delete")))
				{
					Habitat::habitats.erase(Habitat::habitats.begin() + i);
					i--;

					ImGui::EndTabItem();
					continue;
				}

				GUI::Spacing();

				DrawPrefabInput("Prefab ", habitat.prefabFilepath);

				GUI::Spacing();

				ImGui::Text("Required Tiles");
				DrawTileDropdown(habitat.requiredTiles[0], "##1");
				DrawTileDropdown(habitat.requiredTiles[1], "##2");
				DrawTileDropdown(habitat.requiredTiles[2], "##3");

				ImGui::EndTabItem();
			}
		}
		if (ImGui::TabItemButton("+"))
		{
			Habitat::habitats.push_back(HabitatData());
		}
		ImGui::EndTabBar();

		GUI::Spacing(3);
	}

	GUI::Spacing(3);

	if (ImGui::CollapsingHeader("Radial Sprites"))
	{
		GUI::Spacing(3);

		ImGui::BeginTabBar("Types##Radial Sprites");
		if (ImGui::BeginTabItem("Tree"))
		{
			ImGui::PushID("Tree");
			DrawSpriteInput("Regular", HexTile::treeRadialSprites[1]);
			DrawSpriteInput("Hovered", HexTile::treeRadialSprites[0]);
			DrawSpriteInput("Locked ", HexTile::treeRadialSprites[2]);
			ImGui::PopID();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Flower"))
		{
			ImGui::PushID("Flower");
			DrawSpriteInput("Regular", HexTile::flowerRadialSprites[1]);
			DrawSpriteInput("Hovered", HexTile::flowerRadialSprites[0]);
			DrawSpriteInput("Locked ", HexTile::flowerRadialSprites[2]);
			ImGui::PopID();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Water"))
		{
			ImGui::PushID("Water");
			DrawSpriteInput("Regular", HexTile::waterRadialSprites[1]);
			DrawSpriteInput("Hovered", HexTile::waterRadialSprites[0]);
			DrawSpriteInput("Locked ", HexTile::waterRadialSprites[2]);
			ImGui::PopID();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Land"))
		{
			ImGui::PushID("Land");
			DrawSpriteInput("Regular", HexTile::landRadialSprites[1]);
			DrawSpriteInput("Hovered", HexTile::landRadialSprites[0]);
			DrawSpriteInput("Locked ", HexTile::landRadialSprites[2]);
			ImGui::PopID();
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();

		GUI::Spacing(3);
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

void TileEditor::DrawPrefabInput(const char* const name, string& prefabFilepath) noexcept
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

void TileEditor::DrawSpriteInput(const char* const name, string& spriteFilepath) noexcept
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

void TileEditor::DrawTileDropdown(string& tileName, const char* label) noexcept
{
	if (ImGui::BeginCombo(label, tileName.c_str()))
	{
		if (ImGui::BeginMenu("Trees"))
		{
			for (TileData tile : HexTile::trees)
			{
				if (ImGui::Selectable(tile.name.c_str(), tile.name == tileName))
				{
					tileName = tile.name;
				}
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Flowers"))
		{
			for (TileData tile : HexTile::flowers)
			{
				if (ImGui::Selectable(tile.name.c_str(), tile.name == tileName))
				{
					tileName = tile.name;
				}
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Waters"))
		{
			for (TileData tile : HexTile::waters)
			{
				if (ImGui::Selectable(tile.name.c_str(), tile.name == tileName))
				{
					tileName = tile.name;
				}
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Land"))
		{
			for (TileData tile : HexTile::lands)
			{
				if (ImGui::Selectable(tile.name.c_str(), tile.name == tileName))
				{
					tileName = tile.name;
				}
			}
			ImGui::EndMenu();
		}

		ImGui::EndCombo();
	}
}
