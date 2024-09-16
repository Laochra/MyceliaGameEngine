#include "HexEditor.h"

#include "Habitat.h"
#include "HexProgression.h"
#include "HexAudio.h"

vector<TileData>* HexEditor::selectedType = &HexTile::trees;
uint HexEditor::selectedVariant;

void HexEditor::Draw(const char* const name, bool& open) noexcept
{
	ImGui::Begin(name, &open);

	if (!AppInfo::CompareState(AppState::Editor))
	{
		GUI::Spacing(3);
		ImGui::Separator();
		if (AppInfo::CompareState(AppState::Playing))
		{
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "Editing Game Data is disabled while Playing!");
		}
		else
		{
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "Editing Game Data is disabled while Paused!");
		}
		ImGui::Separator();
		GUI::Spacing(3);

		ImGui::BeginDisabled();
	}

	GUI::Spacing(3);

	if (ImGui::Button("Save Changes"))
	{
		const char* const tileDataPath = "Assets\\Tiles\\TileData.json";

		json tileData;
		HexTile::SaveTileDataTo(tileData);
		json progressionData;
		HexProgression::SaveTo(progressionData);
		tileData["Progression"] = progressionData;
		json audioData;
		HexProgression::SaveTo(audioData);
		tileData["Audio"] = audioData;

		ofstream output(tileDataPath);
		output << std::setw(2) << tileData;
	}

	if (!AppInfo::CompareState(AppState::Editor)) ImGui::EndDisabled();

	GUI::Spacing(3);

	if (ImGui::CollapsingHeader("Defaults"))
	{
		if (!AppInfo::CompareState(AppState::Editor)) ImGui::BeginDisabled();

		GUI::Spacing(3);

		DrawPrefabInput("Default", HexTile::defaultTilePath);
		ImGui::TextWrapped("Left behind when moving a habitat");
		GUI::Spacing(3);
		DrawPrefabInput("Empty  ", HexTile::emptyTilePath);
		ImGui::TextWrapped("Empty tile that can be placed on");

		GUI::Spacing(3);

		if (!AppInfo::CompareState(AppState::Editor)) ImGui::EndDisabled();
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

		if (!AppInfo::CompareState(AppState::Editor)) ImGui::BeginDisabled();

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
					if (!AppInfo::CompareState(AppState::Editor)) ImGui::EndDisabled();
					return;
				}
			}
			ImGui::SameLine();
			ImGui::Text(StringBuilder((*selectedType)[selectedVariant].GetTotalPlaced()).CStr());

			GUI::Spacing(3);

			ImGui::Text("Tile Prefabs by Density");
			DrawPrefabInput("Low  ", (*selectedType)[selectedVariant].prefabFilepaths[0]);
			ImGui::SameLine();
			ImGui::Text(StringBuilder((*selectedType)[selectedVariant].countPlaced[0]).CStr());

			DrawPrefabInput("Mid  ", (*selectedType)[selectedVariant].prefabFilepaths[1]);
			ImGui::SameLine();
			ImGui::Text(StringBuilder((*selectedType)[selectedVariant].countPlaced[1]).CStr());

			DrawPrefabInput("High ", (*selectedType)[selectedVariant].prefabFilepaths[2]);
			ImGui::SameLine();
			ImGui::Text(StringBuilder((*selectedType)[selectedVariant].countPlaced[2]).CStr());
		}

		GUI::Spacing(3);

		if (!AppInfo::CompareState(AppState::Editor)) ImGui::EndDisabled();
	}

	GUI::Spacing(3);

	if (ImGui::CollapsingHeader("Progression"))
	{
		if (!AppInfo::CompareState(AppState::Editor)) ImGui::BeginDisabled();

		GUI::Spacing(3);
		
		ImGui::Text(StringBuilder("Current Life: ", HexProgression::GetLife()).CStr());

		GUI::Spacing(3);

		ImGui::PushItemWidth(ImGui::CalcItemWidth() / 5.0f);
		if (ImGui::DragInt("Starting Radius", (int*)&HexProgression::startingRadius, 0.25f, 0, 100, "%d", ImGuiSliderFlags_AlwaysClamp))
		{
			HexProgression::currentRadius = HexProgression::startingRadius;
		}
		ImGui::SameLine();
		GUI::HSpacing(3);
		ImGui::SameLine();
		ImGui::TextColored(ImGui::GetStyle().Colors[ImGuiCol_TextDisabled], StringBuilder("Current: ", HexProgression::currentRadius).CStr());
		ImGui::PopItemWidth();

		GUI::Spacing();
		
		ImGui::Text("Starting Variants");
		std::vector<std::string>& startingVariants = HexProgression::startingVariants;
		ImGui::SameLine();
		GUI::HSpacing(3);
		ImGui::SameLine();
		if (ImGui::Button("Add"))
		{
			startingVariants.push_back("Undefined");
		}

		for (std::vector<std::string>::iterator it = startingVariants.begin(); it < startingVariants.end(); it++)
		{
			DrawTileDropdown(*it, StringBuilder("##", uint(it - startingVariants.begin())).CStr());
			ImGui::SameLine();
			GUI::HSpacing(3);
			ImGui::SameLine();
			if (ImGui::Button("Remove"))
			{
				it = startingVariants.erase(it);
				if (it == startingVariants.end()) break;
			}
		}

		GUI::Spacing(3);

		ImGui::SeparatorText("Life Bonuses");
		ImGui::PushItemWidth(ImGui::CalcItemWidth() / 5.0f);
		ImGui::DragInt("per Tile", (int*)&HexProgression::tileLifeBonus, 0.25f, 0, INT_MAX);
		ImGui::SameLine();
		GUI::HSpacing(3);
		ImGui::SameLine();
		ImGui::DragInt("per Habitat", (int*)&HexProgression::habitatLifeBonus, 0.25f, 0, INT_MAX);
		ImGui::PopItemWidth();

		if (!AppInfo::CompareState(AppState::Editor)) ImGui::EndDisabled();

		GUI::Spacing(3);

		ImGui::SeparatorText("Milestones");

		ImGui::BeginTabBar("Milestones##Milestones");
		for (uint i = 0; i < (uint)HexProgression::lifeMilestones.size(); i++)
		{
			HexProgression::Milestone& lifeMilestone = HexProgression::lifeMilestones[i];

			if (ImGui::BeginTabItem(StringBuilder("  ", i+1, "  ").CStr()))
			{
				if (!AppInfo::CompareState(AppState::Editor)) ImGui::BeginDisabled();
				const char* typeName;
				switch (lifeMilestone.type)
				{
				default:
				case HexProgression::Milestone::VariantUnlock: typeName = "Variant Unlock"; break;
				case HexProgression::Milestone::AddedEffects: typeName = "Added Effects"; break;
				}
				if (ImGui::BeginCombo("Type", typeName))
				{
					if (ImGui::Selectable("Variant Unlock", lifeMilestone.type == HexProgression::Milestone::VariantUnlock))
					{
						lifeMilestone.type = HexProgression::Milestone::VariantUnlock;
					}
					if (ImGui::Selectable("Added Effects", lifeMilestone.type == HexProgression::Milestone::AddedEffects))
					{
						lifeMilestone.type = HexProgression::Milestone::AddedEffects;
					}
					ImGui::EndCombo();
				}

				ImGui::SameLine();
				GUI::HSpacing(3);
				ImGui::SameLine();
				if ((ImGui::Button("Delete")))
				{
					HexProgression::lifeMilestones.erase(HexProgression::lifeMilestones.begin() + i);
					i--;

					ImGui::EndTabItem();
					continue;
				}

				GUI::Spacing();

				ImGui::PushItemWidth(ImGui::CalcItemWidth() / 5.0f);
				ImGui::DragInt("Life Requirement", (int*)&lifeMilestone.lifeRequirement, 0.25f, 0, INT_MAX);
				ImGui::SameLine();
				GUI::HSpacing(3);
				ImGui::SameLine();
				ImGui::DragInt("Radius Increase", (int*)&lifeMilestone.radiusIncrease, 0.25f, 0, 16U);
				ImGui::PopItemWidth();

				GUI::Spacing();

				if (lifeMilestone.type == HexProgression::Milestone::VariantUnlock)
				{
					ImGui::Text("Unlocked Variants");
					std::vector<std::string>& unlockedVariants = lifeMilestone.names;
					ImGui::SameLine();
					GUI::HSpacing(3);
					ImGui::SameLine();
					if (ImGui::Button("Add"))
					{
						unlockedVariants.push_back("Undefined");
					}

					for (std::vector<std::string>::iterator it = unlockedVariants.begin(); it < unlockedVariants.end(); it++)
					{
						DrawTileDropdown(*it, StringBuilder("##", uint(it - unlockedVariants.begin())).CStr());
						ImGui::SameLine();
						GUI::HSpacing(3);
						ImGui::SameLine();
						if (ImGui::Button("Remove"))
						{
							it = unlockedVariants.erase(it);
							if (it == unlockedVariants.end()) break;
						}
					}
				}
				if (!AppInfo::CompareState(AppState::Editor)) ImGui::EndDisabled();
				ImGui::EndTabItem();
			}
		}
		if (!AppInfo::CompareState(AppState::Editor)) ImGui::BeginDisabled();
		if (ImGui::TabItemButton("+"))
		{
			HexProgression::lifeMilestones.push_back(HexProgression::Milestone());
		}
		if (!AppInfo::CompareState(AppState::Editor)) ImGui::EndDisabled();
		ImGui::EndTabBar();

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
				if (!AppInfo::CompareState(AppState::Editor)) ImGui::BeginDisabled();

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

				if (!AppInfo::CompareState(AppState::Editor)) ImGui::EndDisabled();

				ImGui::EndTabItem();
			}
		}
		if (!AppInfo::CompareState(AppState::Editor)) ImGui::BeginDisabled();
		if (ImGui::TabItemButton("+"))
		{
			Habitat::habitats.push_back(HabitatData());
		}
		if (!AppInfo::CompareState(AppState::Editor)) ImGui::EndDisabled();
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
			if (!AppInfo::CompareState(AppState::Editor)) ImGui::BeginDisabled();
			ImGui::PushID("Tree");
			DrawSpriteInput("Regular", HexTile::treeRadialSprites[0]);
			DrawSpriteInput("Hovered", HexTile::treeRadialSprites[1]);
			DrawSpriteInput("Locked ", HexTile::treeRadialSprites[2]);
			ImGui::PopID();
			if (!AppInfo::CompareState(AppState::Editor)) ImGui::EndDisabled();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Flower"))
		{
			if (!AppInfo::CompareState(AppState::Editor)) ImGui::BeginDisabled();
			ImGui::PushID("Flower");
			DrawSpriteInput("Regular", HexTile::flowerRadialSprites[0]);
			DrawSpriteInput("Hovered", HexTile::flowerRadialSprites[1]);
			DrawSpriteInput("Locked ", HexTile::flowerRadialSprites[2]);
			ImGui::PopID();
			if (!AppInfo::CompareState(AppState::Editor)) ImGui::EndDisabled();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Water"))
		{
			if (!AppInfo::CompareState(AppState::Editor)) ImGui::BeginDisabled();
			ImGui::PushID("Water");
			DrawSpriteInput("Regular", HexTile::waterRadialSprites[0]);
			DrawSpriteInput("Hovered", HexTile::waterRadialSprites[1]);
			DrawSpriteInput("Locked ", HexTile::waterRadialSprites[2]);
			ImGui::PopID();
			if (!AppInfo::CompareState(AppState::Editor)) ImGui::EndDisabled();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Land"))
		{
			if (!AppInfo::CompareState(AppState::Editor)) ImGui::BeginDisabled();
			ImGui::PushID("Land");
			DrawSpriteInput("Regular", HexTile::landRadialSprites[0]);
			DrawSpriteInput("Hovered", HexTile::landRadialSprites[1]);
			DrawSpriteInput("Locked ", HexTile::landRadialSprites[2]);
			ImGui::PopID();
			if (!AppInfo::CompareState(AppState::Editor)) ImGui::EndDisabled();
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();

		GUI::Spacing(3);
	}

	GUI::Spacing(3);

	if (ImGui::CollapsingHeader("Audio"))
	{
		GUI::Spacing(3);

		ImGui::BeginTabBar("Audio##Audio Types");
		if (ImGui::BeginTabItem("Music"))
		{
			if (!AppInfo::CompareState(AppState::Editor)) ImGui::BeginDisabled();
			GUI::Spacing(3);

			ImGui::DragFloat("Volume", &HexAudio::musicVolume, 0.01f, 0.0f, 10.0f);
			
			GUI::Spacing(3);

			DrawAudioInput("##MusicTrack", HexAudio::music);

			if (!AppInfo::CompareState(AppState::Editor)) ImGui::EndDisabled();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Ambience"))
		{
			if (!AppInfo::CompareState(AppState::Editor)) ImGui::BeginDisabled();
			GUI::Spacing(3);

			ImGui::DragFloat("Volume", &HexAudio::ambienceVolume, 0.01f, 0.0f, 10.0f);

			GUI::Spacing(3);

			if (ImGui::Button("Add"))
			{
				HexAudio::ambience.push_back("None");
			}
			for (auto it = HexAudio::ambience.begin(); it != HexAudio::ambience.end(); it++)
			{
				DrawAudioInput(StringBuilder("##", it - HexAudio::ambience.begin()).CStr(), *it);
				ImGui::SameLine();
				GUI::HSpacing(3);
				ImGui::SameLine();
				if (ImGui::Button("Delete"))
				{
					it = HexAudio::ambience.erase(it);
					if (it == HexAudio::ambience.end()) break;
				}
			}

			if (!AppInfo::CompareState(AppState::Editor)) ImGui::EndDisabled();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Spirit SFX"))
		{
			if (!AppInfo::CompareState(AppState::Editor)) ImGui::BeginDisabled();
			GUI::Spacing(3);

			ImGui::DragFloat("Volume", &HexAudio::spiritSFXVolume, 0.01f, 0.0f, 10.0f);

			GUI::Spacing(3);

			for (auto it = HexAudio::spiritSounds.begin(); it != HexAudio::spiritSounds.end(); it++)
			{
				bool habitatExists = false;
				for (HabitatData& habitatData : Habitat::habitats)
				{
					if (it->first == habitatData.name)
					{
						habitatExists = true;
						break;
					}
				}
				if (!habitatExists) it = HexAudio::spiritSounds.erase(it);
			}

			for (HabitatData& habitatData : Habitat::habitats)
			{
				auto it = HexAudio::spiritSounds.find(habitatData.name);
				if (it == HexAudio::spiritSounds.end())
				{
					it = HexAudio::spiritSounds.insert(std::pair(habitatData.name, "None")).first;
				}

				DrawAudioInput(it->first.c_str(), it->second);
			}

			if (!AppInfo::CompareState(AppState::Editor)) ImGui::EndDisabled();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Misc SFX"))
		{
			if (!AppInfo::CompareState(AppState::Editor)) ImGui::BeginDisabled();
			GUI::Spacing(3);

			ImGui::DragFloat("Volume", &HexAudio::sfxVolume, 0.01f, 0.0f, 10.0f);

			GUI::Spacing(3);

			using SoundEffect = HexAudio::SoundEffect;

			ImGui::PushID("TilePlacement");
			ImGui::Text("Tile Placement");
			DrawAudioInput("Tree", HexAudio::soundEffects[SoundEffect::PlaceTree]);
			DrawAudioInput("Flower", HexAudio::soundEffects[SoundEffect::PlaceFlower]);
			DrawAudioInput("Water", HexAudio::soundEffects[SoundEffect::PlaceWater]);
			DrawAudioInput("Land", HexAudio::soundEffects[SoundEffect::PlaceLand]);
			GUI::Spacing();
			DrawAudioInput("Habitat", HexAudio::soundEffects[SoundEffect::FormHabitat]);
			ImGui::PopID();

			GUI::Spacing(3);

			ImGui::PushID("RadialMenu");
			ImGui::Text("Radial Menu");
			DrawAudioInput("Open", HexAudio::soundEffects[SoundEffect::RadialOpen]);
			DrawAudioInput("Close", HexAudio::soundEffects[SoundEffect::RadialClose]);
			DrawAudioInput("Cycle Page", HexAudio::soundEffects[SoundEffect::RadialCyclePage]);
			DrawAudioInput("Cycle Density", HexAudio::soundEffects[SoundEffect::RadialCycleDensity]);
			ImGui::PopID();

			GUI::Spacing(3);

			ImGui::PushID("Scrapbook");
			ImGui::Text("Scrapbook");
			DrawAudioInput("Open", HexAudio::soundEffects[SoundEffect::ScrapbookOpen]);
			DrawAudioInput("Close", HexAudio::soundEffects[SoundEffect::ScrapbookClose]);
			DrawAudioInput("Page Turn", HexAudio::soundEffects[SoundEffect::ScrapbookTurnPage]);
			DrawAudioInput("Notification", HexAudio::soundEffects[SoundEffect::ScrapbookNotification]);
			DrawAudioInput("New Habitat", HexAudio::soundEffects[SoundEffect::ScrapbookNewHabitatPage]);
			ImGui::PopID();

			if (!AppInfo::CompareState(AppState::Editor)) ImGui::EndDisabled();
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();

		GUI::Spacing(3);
	}

	ImGui::End();
}

void HexEditor::DrawType(vector<TileData>& type) noexcept
{	
	if (&type != selectedType)
	{
		selectedType = &type;
		selectedVariant = 0U;
	}

	if (!AppInfo::CompareState(AppState::Editor)) ImGui::BeginDisabled();

	if (ImGui::Button("Add"))
	{
		type.push_back(TileData("New Variant"));
	}

	if (!AppInfo::CompareState(AppState::Editor)) ImGui::EndDisabled();

	for (uint i = 0; i < (uint)type.size(); i++)
	{
		bool isSelected = i == selectedVariant;

		if (ImGui::Selectable((" " + type[i].name + "##" + std::to_string(i)).c_str(), isSelected))
		{
			selectedVariant = i;
		}
	}
}

void HexEditor::DrawPrefabInput(const char* const name, string& prefabFilepath) noexcept
{
	if (ImGui::Button(StringBuilder("Load##", name).CStr()))
	{
		using namespace FileDialogue;

		string path = GetLoadPath(PathDetails("Tile Prefab", "Assets\\", { "*.prefab" }), LimitToAssetFolder::True);
		if (path.size() > 0)
		{
			prefabFilepath = path;
		}
	}

	ImGui::SameLine();

	ImGui::BeginDisabled();
	ImGui::InputText(name, &prefabFilepath, ImGuiInputTextFlags_ReadOnly);
	ImGui::EndDisabled();
}

void HexEditor::DrawSpriteInput(const char* const name, string& spriteFilepath) noexcept
{
	if (ImGui::Button(StringBuilder("Load##", name).CStr()))
	{
		using namespace FileDialogue;

		string path = GetLoadPath(PathDetails("Radial Sprite", "Assets\\", { "*.png", "*.tga", "*.jpg" }), LimitToAssetFolder::True);
		if (path.size() > 0)
		{
			spriteFilepath = path;
		}
	}

	ImGui::SameLine();

	ImGui::BeginDisabled();
	ImGui::InputText(name, &spriteFilepath, ImGuiInputTextFlags_ReadOnly);
	ImGui::EndDisabled();
}

void HexEditor::DrawAudioInput(const char* const name, string& audioFilepath) noexcept
{
	if (ImGui::Button(StringBuilder("Load##", name).CStr()))
	{
		using namespace FileDialogue;

		string path = GetLoadPath(PathDetails("Audio File", "Assets\\", { "*.wav", "*.mp3", "*.ogg" }), LimitToAssetFolder::True);
		if (path.size() > 0)
		{
			audioFilepath = path;
		}
	}

	ImGui::SameLine();

	ImGui::BeginDisabled();
	ImGui::InputText(name, &audioFilepath, ImGuiInputTextFlags_ReadOnly);
	ImGui::EndDisabled();
}

void HexEditor::DrawTileDropdown(string& tileName, const char* label) noexcept
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
