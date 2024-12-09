#include "HexEditor.h"

#include "TileData.h"
#include "HabitatData.h"
#include "SpiritData.h"

#include "HexGameInfo.h"
#include "HexRadial.h"
#include "HexScrapbook.h"
#include "HexProgression.h"
#include "HexCameraData.h"
#include "HexAudio.h"
#include "HexFog.h"
#include "HexClouds.h"
#include "HexMenus.h"

#include "UIManager.h"
#include "LightingManager.h"

#include "HexGameSerialiser.h"

vector<TileData>* HexEditor::selectedType = &TileData::Get(HexType::Tree);
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
		const char* const dataPath = "Assets\\HexGameData.json";

		json dataJSON;
		
		HexGameSerialiser::SaveDataTo(dataJSON);

		ofstream output(dataPath);
		output << std::setw(2) << dataJSON;
	}

	if (!AppInfo::CompareState(AppState::Editor)) ImGui::EndDisabled();

	GUI::Spacing(3);

	if (ImGui::CollapsingHeader("Menu Sprites"))
	{
		if (!AppInfo::CompareState(AppState::Editor)) ImGui::BeginDisabled();

		DrawUISpriteDropdown("MainMenu", HexMenus::mainMenu);
		DrawUISpriteDropdown("PauseMenu", HexMenus::pauseMenu);
		DrawUISpriteDropdown("Continue", HexMenus::continueScreen);
		DrawUISpriteDropdown("Credits", HexMenus::creditsScreen);

		if (!AppInfo::CompareState(AppState::Editor)) ImGui::EndDisabled();
	}

	GUI::Spacing(3);

	if (ImGui::CollapsingHeader("Tiles"))
	{
		GUI::Spacing(3);
		ImGui::Indent();

		if (ImGui::CollapsingHeader("Required"))
		{
			if (!AppInfo::CompareState(AppState::Editor)) ImGui::BeginDisabled();

			GUI::Spacing(3);

			DrawPrefabInput("Default", TileData::defaultTilePaths[0]);
			ImGui::TextWrapped("Left behind when moving a habitat");
			GUI::Spacing(3);
			DrawPrefabInput("Perimeter  ", TileData::defaultTilePaths[1]);
			ImGui::TextWrapped("Empty tile that can be placed on");
			GUI::Spacing(3);
			DrawPrefabInput("Mother Tree  ", TileData::defaultTilePaths[2]);
			ImGui::TextWrapped("Starting Tile");

			GUI::Spacing(3);

			if (!AppInfo::CompareState(AppState::Editor)) ImGui::EndDisabled();
		}

		GUI::Spacing(3);

		if (ImGui::CollapsingHeader("Variants"))
		{
			GUI::Spacing(3);

			ImGui::BeginTabBar("Types##Tile Variants");
			if (ImGui::BeginTabItem("Tree"))
			{
				DrawType(TileData::Get(HexType::Tree));
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Flower"))
			{
				DrawType(TileData::Get(HexType::Flower));
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Water"))
			{
				DrawType(TileData::Get(HexType::Water));
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Land"))
			{
				DrawType(TileData::Get(HexType::Land));
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
				ImGui::Text(StringBuilder((*selectedType)[selectedVariant].countPlaced).CStr());

				GUI::Spacing(3);

				DrawPrefabInput("Prefab", (*selectedType)[selectedVariant].prefabFilepath);

				GUI::Spacing(3);

				DrawSpriteInput("Sprite", (*selectedType)[selectedVariant].spriteFilepath);
			}

			GUI::Spacing(3);

			DrawUISpriteDropdown("Selected Tile", TileData::selectedTile);

			GUI::Spacing(3);
			if (!AppInfo::CompareState(AppState::Editor)) ImGui::EndDisabled();
		}

		ImGui::Unindent();
		GUI::Spacing(3);
	}

	GUI::Spacing(3);

	if (ImGui::CollapsingHeader("Progression"))
	{
		if (!AppInfo::CompareState(AppState::Editor)) ImGui::BeginDisabled();

		GUI::Spacing(3);
		
		ImGui::Text(StringBuilder("Current Life: ", HexProgression::GetLife()).CStr());

		GUI::Spacing(3);

		DrawUISpriteDropdown("Notification Icon", HexProgression::newVariantsIcon);

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
		std::vector<TileID>& startingVariants = HexProgression::startingVariants;
		ImGui::SameLine();
		GUI::HSpacing(3);
		ImGui::SameLine();
		if (ImGui::Button("Add"))
		{
			startingVariants.push_back(TileID());
		}

		for (std::vector<TileID>::iterator it = startingVariants.begin(); it < startingVariants.end();)
		{
			uint id = uint(it - startingVariants.begin());
			ImGui::PushID(id);
			DrawTileDropdown(StringBuilder("##", id).CStr(), *it);
			ImGui::SameLine();
			GUI::HSpacing(3);
			ImGui::SameLine();
			if (ImGui::Button("Remove"))
			{
				it = startingVariants.erase(it);
				if (it == startingVariants.end())
				{
					ImGui::PopID();
					break;
				}
			}
			else
			{
				it++;
			}
			ImGui::PopID();
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
				ImGui::BeginDisabled();
				ImGui::DragInt("Radius Increase", (int*)&lifeMilestone.radiusIncrease, 0.25f, 0, 16U);
				ImGui::EndDisabled();
				ImGui::PopItemWidth();

				GUI::Spacing();

				
				ImGui::Text("Unlocked Variants");
				std::vector<TileID>& unlockedVariants = lifeMilestone.variantUnlocks;
				ImGui::SameLine();
				GUI::HSpacing(3);
				ImGui::SameLine();
				if (ImGui::Button("Add"))
				{
					unlockedVariants.push_back(TileID());
				}

				for (std::vector<TileID>::iterator it = unlockedVariants.begin(); it < unlockedVariants.end();)
				{
					DrawTileDropdown(StringBuilder("##", uint(it - unlockedVariants.begin())).CStr(), *it);
					ImGui::SameLine();
					GUI::HSpacing(3);
					ImGui::SameLine();
					if (ImGui::Button("Remove"))
					{
						it = unlockedVariants.erase(it);
						if (it == unlockedVariants.end()) break;
					}
					else
					{
						it++;
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

	if (ImGui::CollapsingHeader("Fog"))
	{
		if (AppInfo::CompareState(AppState::Playing)) ImGui::BeginDisabled();

		GUI::Spacing(3);

		ImGui::DragFloat("Radius", &HexFog::currentRadius, 0.01f, -0.5f, FLT_MAX, "%.3f", ImGuiSliderFlags_AlwaysClamp);
		GUI::Spacing();
		ImGui::DragFloat("Range", &HexFog::gradientRange, 0.001f, 0.001f, FLT_MAX, "%.3f", ImGuiSliderFlags_AlwaysClamp);
		ImGui::DragFloat("Speed", &HexFog::animationSpeed, 0.001f, 0.001f, FLT_MAX, "%.3f", ImGuiSliderFlags_AlwaysClamp);

		GUI::Spacing(3);

		if (DrawHDRTextureInput("Texture", HexFog::fogTextureFilepath))
		{
			HexFog::Load(HexFog::GetFilter());
		}

		const char* filterName;
		switch (HexFog::GetFilter())
		{
		default:
			filterName = "None";
			break;
		case Texture::Filter::Bilinear:
			filterName = "Bilinear";
			break;
		case Texture::Filter::Trilinear:
			filterName = "Trilinear";
			break;
		}
		if (ImGui::BeginCombo("Texture Filter", filterName))
		{
			if (ImGui::Selectable("None", HexFog::GetFilter() == Texture::Filter::None))
			{
				HexFog::Load(Texture::Filter::None);
			}
			if (ImGui::Selectable("Bilinear", HexFog::GetFilter() == Texture::Filter::Bilinear))
			{
				HexFog::Load(Texture::Filter::Bilinear);
			}
			if (ImGui::Selectable("Trilinear", HexFog::GetFilter() == Texture::Filter::Trilinear))
			{
				HexFog::Load(Texture::Filter::Trilinear);
			}

			ImGui::EndCombo();
		}

		GUI::Spacing(3);

		string cachedBackgroundPath = HexGameInfo::backgroundFilepath;
		if (DrawPrefabInput("Background", cachedBackgroundPath))
		{
			HexGameInfo::LoadBackgroundPrefab(cachedBackgroundPath);
		}

		GUI::Spacing(3);

		if (AppInfo::CompareState(AppState::Playing)) ImGui::EndDisabled();
	}

	GUI::Spacing(3);

	if (ImGui::CollapsingHeader("Clouds"))
	{
		if (AppInfo::CompareState(AppState::Playing)) ImGui::BeginDisabled();

		GUI::Spacing(3);

		if (ImGui::Button("Add"))
		{
			HexClouds::prefabFilepaths.push_back(string("None"));
			HexClouds::prefabs.push_back(json());
		}

		for (int i = 0; i < (int)HexClouds::prefabFilepaths.size(); i++)
		{
			string cachedPrefabPath = HexClouds::prefabFilepaths[i];
			if (DrawPrefabInput(StringBuilder("##", i).CStr(), cachedPrefabPath))
			{
				HexClouds::LoadPrefab(i, cachedPrefabPath);
			}
			ImGui::SameLine();
			if (ImGui::Button(StringBuilder("Delete##", i).CStr()))
			{
				HexClouds::prefabFilepaths.erase(HexClouds::prefabFilepaths.begin() + i);
				HexClouds::prefabs.erase(HexClouds::prefabs.begin() + i);
				i--;
				continue;
			}
		}

		GUI::Spacing(3);

		if (AppInfo::CompareState(AppState::Playing)) ImGui::EndDisabled();
	}

	GUI::Spacing(3);

	if (ImGui::CollapsingHeader("Habitats"))
	{
		GUI::Spacing(3);

		ImGui::BeginTabBar("Habitats##Habitats");
		for (uint i = 0; i < (uint)HabitatData::habitatsData.size(); i++)
		{
			HabitatData& habitat = HabitatData::habitatsData[i];

			if (ImGui::BeginTabItem(StringBuilder(habitat.name, "###", i).CStr()))
			{
				if (!AppInfo::CompareState(AppState::Editor)) ImGui::BeginDisabled();

				ImGui::InputText("Name ", &habitat.name);

				ImGui::SameLine();
				if ((ImGui::Button("Delete")))
				{
					HabitatData::habitatsData.erase(HabitatData::habitatsData.begin() + i);
					i--;

					ImGui::EndTabItem();
					continue;
				}

				GUI::Spacing();

				DrawPrefabInput("Prefab ", habitat.prefabFilepath);

				GUI::Spacing();

				ImGui::Text("Required Tiles");
				DrawTileDropdown("##1", habitat.requiredTiles[0]);
				DrawTileDropdown("##2", habitat.requiredTiles[1]);
				DrawTileDropdown("##3", habitat.requiredTiles[2]);

				if (!AppInfo::CompareState(AppState::Editor)) ImGui::EndDisabled();

				ImGui::EndTabItem();
			}
		}
		if (!AppInfo::CompareState(AppState::Editor)) ImGui::BeginDisabled();
		if (ImGui::TabItemButton("+"))
		{
			HabitatData::habitatsData.push_back(HabitatData());
		}
		if (!AppInfo::CompareState(AppState::Editor)) ImGui::EndDisabled();
		ImGui::EndTabBar();

		GUI::Spacing(3);
	}

	GUI::Spacing(3);

	if (ImGui::CollapsingHeader("Spirits"))
	{
		GUI::Spacing(3);

		ImGui::BeginTabBar("Spirits##Spirits");
		for (int i = 0; i < (int)HabitatData::habitatsData.size(); i++)
		{
			if (SpiritData::spiritsData.size() < HabitatData::habitatsData.size())
			{
				SpiritData::spiritsData.push_back(SpiritData());
			}
			
			SpiritData& spirit = SpiritData::spiritsData[i];

			spirit.name = HabitatData::habitatsData[i].name;

			if (ImGui::BeginTabItem(HabitatData::habitatsData[i].name.c_str()))
			{
				if (!AppInfo::CompareState(AppState::Editor)) ImGui::BeginDisabled();
				ImGui::PushID(HabitatData::habitatsData[i].name.c_str());

				DrawPrefabInput("Prefab ", spirit.prefabFilepath);

				GUI::Spacing(3);

				ImGui::PopID();
				if (!AppInfo::CompareState(AppState::Editor)) ImGui::EndDisabled();
				ImGui::EndTabItem();
			}

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
			if (!AppInfo::CompareState(AppState::Editor)) ImGui::BeginDisabled();
			ImGui::PushID("Tree");
			DrawSpriteInput("Regular", HexRadial::treeRadialSprites[0]);
			DrawSpriteInput("Hovered", HexRadial::treeRadialSprites[1]);
			DrawSpriteInput("Locked ", HexRadial::treeRadialSprites[2]);
			ImGui::PopID();
			if (!AppInfo::CompareState(AppState::Editor)) ImGui::EndDisabled();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Flower"))
		{
			if (!AppInfo::CompareState(AppState::Editor)) ImGui::BeginDisabled();
			ImGui::PushID("Flower");
			DrawSpriteInput("Regular", HexRadial::flowerRadialSprites[0]);
			DrawSpriteInput("Hovered", HexRadial::flowerRadialSprites[1]);
			DrawSpriteInput("Locked ", HexRadial::flowerRadialSprites[2]);
			ImGui::PopID();
			if (!AppInfo::CompareState(AppState::Editor)) ImGui::EndDisabled();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Water"))
		{
			if (!AppInfo::CompareState(AppState::Editor)) ImGui::BeginDisabled();
			ImGui::PushID("Water");
			DrawSpriteInput("Regular", HexRadial::waterRadialSprites[0]);
			DrawSpriteInput("Hovered", HexRadial::waterRadialSprites[1]);
			DrawSpriteInput("Locked ", HexRadial::waterRadialSprites[2]);
			ImGui::PopID();
			if (!AppInfo::CompareState(AppState::Editor)) ImGui::EndDisabled();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Land"))
		{
			if (!AppInfo::CompareState(AppState::Editor)) ImGui::BeginDisabled();
			ImGui::PushID("Land");
			DrawSpriteInput("Regular", HexRadial::landRadialSprites[0]);
			DrawSpriteInput("Hovered", HexRadial::landRadialSprites[1]);
			DrawSpriteInput("Locked ", HexRadial::landRadialSprites[2]);
			ImGui::PopID();
			if (!AppInfo::CompareState(AppState::Editor)) ImGui::EndDisabled();
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();

		GUI::Spacing(3);
	}

	GUI::Spacing(3);

	if (ImGui::CollapsingHeader("Scrapbook"))
	{
		GUI::Spacing(3);
		ImGui::Indent();

		if (ImGui::CollapsingHeader("Sprites"))
		{
			ImGui::Text("Scrapbook");
			DrawUISpriteDropdown("UI Reference", HexScrapbook::base);
			GUI::Spacing();
			DrawSpriteInput("Sprite", HexScrapbook::baseTexture);

			GUI::Spacing(3);

			ImGui::BeginTabBar("Habitats##Scrapbook Sprites");
			for (int i = 0; i < (int)HabitatData::habitatsData.size(); i++)
			{
				if (HexScrapbook::habitats.size() < HabitatData::habitatsData.size())
				{
					HexScrapbook::habitats.push_back(HexScrapbook::HabitatCollection());
				}
				if (ImGui::BeginTabItem(HabitatData::habitatsData[i].name.c_str()))
				{
					if (!AppInfo::CompareState(AppState::Editor)) ImGui::BeginDisabled();
					ImGui::PushID(HabitatData::habitatsData[i].name.c_str());

					ImGui::PushID("Habitat");
					ImGui::Text("Habitat");
					DrawUISpriteDropdown("UI Reference", HexScrapbook::habitats[i].habitat);
					GUI::Spacing();
					DrawSpriteInput("Locked", HexScrapbook::habitats[i].habitatTextures[0]);
					DrawSpriteInput("Unlocked", HexScrapbook::habitats[i].habitatTextures[1]);
					DrawSpriteInput("Recieved", HexScrapbook::habitats[i].habitatTextures[2]);
					ImGui::PopID();

					GUI::Spacing(3);

					ImGui::PushID("Tile 0");
					ImGui::Text(StringBuilder("Tile 0: ", TileData::Get(HabitatData::habitatsData[i].requiredTiles[0]).name).CStr());
					DrawUISpriteDropdown("UI Reference", HexScrapbook::habitats[i].tiles[0]);
					GUI::Spacing();
					DrawSpriteInput("Locked", HexScrapbook::habitats[i].tileTextures[0]);
					DrawSpriteInput("Unlocked", HexScrapbook::habitats[i].tileTextures[1]);
					DrawSpriteInput("Recieved", HexScrapbook::habitats[i].tileTextures[2]);
					ImGui::PopID();
					GUI::Spacing(3);
					ImGui::PushID("Tile 1");
					ImGui::Text(StringBuilder("Tile 1: ", TileData::Get(HabitatData::habitatsData[i].requiredTiles[1]).name).CStr());
					DrawUISpriteDropdown("UI Reference", HexScrapbook::habitats[i].tiles[1]);
					GUI::Spacing();
					DrawSpriteInput("Locked", HexScrapbook::habitats[i].tileTextures[3]);
					DrawSpriteInput("Unlocked", HexScrapbook::habitats[i].tileTextures[4]);
					DrawSpriteInput("Recieved", HexScrapbook::habitats[i].tileTextures[5]);
					ImGui::PopID();
					GUI::Spacing(3);
					ImGui::PushID("Tile 2");
					ImGui::Text(StringBuilder("Tile 2: ", TileData::Get(HabitatData::habitatsData[i].requiredTiles[2]).name).CStr());
					DrawUISpriteDropdown("UI Reference", HexScrapbook::habitats[i].tiles[2]);
					GUI::Spacing();
					DrawSpriteInput("Locked", HexScrapbook::habitats[i].tileTextures[6]);
					DrawSpriteInput("Unlocked", HexScrapbook::habitats[i].tileTextures[7]);
					DrawSpriteInput("Recieved", HexScrapbook::habitats[i].tileTextures[8]);
					ImGui::PopID();

					GUI::Spacing(3);

					ImGui::PopID();
					if (!AppInfo::CompareState(AppState::Editor)) ImGui::EndDisabled();
					ImGui::EndTabItem();
				}

			}

			ImGui::EndTabBar();
		}

		GUI::Spacing(3);

		if (ImGui::CollapsingHeader("Sticker Event"))
		{
			HexProgression::StickerEventData& stickerEvent = HexProgression::stickerEvent;

			ImGui::SeparatorText("Sticker Movement");
			ImGui::DragFloat("Amount", &stickerEvent.moveAmount);
			ImGui::DragFloat("Speed", &stickerEvent.moveSpeed);

			ImGui::SeparatorText("Timing Delays");
			ImGui::DragFloat("Open Sheet", &stickerEvent.startDelay);
			ImGui::DragFloat("Place Sticker", &stickerEvent.stickerPlaceDelay);
			ImGui::DragFloat("Close Sheet", &stickerEvent.closeDelay);
			ImGui::DragFloat("Expand Border", &stickerEvent.expandBorderDelay);
			ImGui::DragFloat("End", &stickerEvent.endDelay);
		}

		ImGui::Unindent();
		GUI::Spacing(3);
	}

	GUI::Spacing(3);

	if (ImGui::CollapsingHeader("Camera"))
	{
		GUI::Spacing(3);
		ImGui::BeginDisabled(!AppInfo::CompareState(AppState::Editor));

		ImGui::PushItemWidth(ImGui::CalcItemWidth() / 2.0f);
		if (ImGui::DragFloat("##OffsetDirectionY", &HexCameraData::offsetDirection.x, 0.005f, 0, 1, "%.3f", ImGuiSliderFlags_AlwaysClamp))
		{
			HexCameraData::offsetDirection.y = sin(acos(HexCameraData::offsetDirection.x));
		}
		ImGui::SameLine();
		GUI::HSpacing(3);
		ImGui::SameLine();
		if (ImGui::DragFloat("##OffsetDirectionZ", &HexCameraData::offsetDirection.y, 0.005f, 0, 1, "%.3f", ImGuiSliderFlags_AlwaysClamp))
		{
			HexCameraData::offsetDirection.x = cos(asin(HexCameraData::offsetDirection.y));
		}
		ImGui::PopItemWidth();
		ImGui::SameLine();
		GUI::HSpacing(3);
		ImGui::SameLine();
		ImGui::Text("Offset Direction");

		GUI::Spacing();

		ImGui::DragFloat("Max Zoom", &HexCameraData::maxZoom);
		ImGui::DragFloat("Min Zoom", &HexCameraData::minZoom);
		ImGui::DragFloat("Start Zoom", &HexCameraData::startZoom);
		ImGui::Text(StringBuilder("Current Zoom: ", HexCameraData::currentZoom).CStr());

		ImGui::EndDisabled();
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
			for (auto it = HexAudio::ambience.begin(); it != HexAudio::ambience.end();)
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
				else
				{
					it++;
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
				for (HabitatData& habitatData : HabitatData::habitatsData)
				{
					if (it->first == habitatData.name)
					{
						habitatExists = true;
						break;
					}
				}
				if (!habitatExists) it = HexAudio::spiritSounds.erase(it);
			}

			for (HabitatData& habitatData : HabitatData::habitatsData)
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

			ImGui::PushID("Progression");
			ImGui::Text("Progression");
			DrawAudioInput("VariantUnlock", HexAudio::soundEffects[SoundEffect::VariantUnlock]);
			DrawAudioInput("FogMove", HexAudio::soundEffects[SoundEffect::FogMove]);
			ImGui::PopID();

			GUI::Spacing(3);

			ImGui::PushID("RadialMenu");
			ImGui::Text("Radial Menu");
			DrawAudioInput("Open", HexAudio::soundEffects[SoundEffect::RadialOpen]);
			DrawAudioInput("Close", HexAudio::soundEffects[SoundEffect::RadialClose]);
			DrawAudioInput("Select", HexAudio::soundEffects[SoundEffect::RadialSelect]);
			DrawAudioInput("Hover", HexAudio::soundEffects[SoundEffect::RadialHover]);
			DrawAudioInput("Cycle Page", HexAudio::soundEffects[SoundEffect::RadialCyclePage]);
			ImGui::PopID();

			GUI::Spacing(3);

			ImGui::PushID("Scrapbook");
			ImGui::Text("Scrapbook");
			DrawAudioInput("Open", HexAudio::soundEffects[SoundEffect::ScrapbookOpen]);
			DrawAudioInput("Close", HexAudio::soundEffects[SoundEffect::ScrapbookClose]);
			DrawAudioInput("Sticker", HexAudio::soundEffects[SoundEffect::ScrapbookStickerPlace]);
			ImGui::PopID();

			GUI::Spacing(3);

			DrawAudioInput("MenuSelect", HexAudio::soundEffects[SoundEffect::MenuSelect]);

			if (!AppInfo::CompareState(AppState::Editor)) ImGui::EndDisabled();
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();

		GUI::Spacing(3);
	}

	GUI::Spacing(3);

	if (ImGui::CollapsingHeader("Lighting"))
	{
		if (DrawPrefabInput("Light Object", HexGameInfo::lightingFilepath))
		{
			HexGameInfo::LoadLightingPrefab(HexGameInfo::lightingFilepath);
		}

		ImGui::SeparatorText("Ambient");
		ImGui::ColorEdit3("Colour", (float*)&LightingManager::ambientLight.colour);
		ImGui::DragFloat("Intensity", (float*)&LightingManager::ambientLight.intensity);
	}

	GUI::Spacing(3);

	if (ImGui::CollapsingHeader("Misc Colours"))
	{
		GUI::Spacing(3);

		ImGui::DragFloat4("Highlight", (float*)&HexGameInfo::highlightColour, 0.01f, 0.0f, FLT_MAX);

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

bool HexEditor::DrawPrefabInput(const char* const name, string& prefabFilepath) noexcept
{
	bool modified = 0;

	if (ImGui::Button(StringBuilder("Load##", name).CStr()))
	{
		using namespace FileDialogue;

		string path = GetLoadPath(PathDetails("Prefab", "Assets\\", { "*.prefab" }), LimitToAssetFolder::True);
		if (path.size() > 0)
		{
			prefabFilepath = path;
			modified = true;
		}
	}

	ImGui::SameLine();

	ImGui::BeginDisabled();
	ImGui::InputText(name, &prefabFilepath, ImGuiInputTextFlags_ReadOnly);
	ImGui::EndDisabled();

	return modified;
}

void HexEditor::DrawSpriteInput(const char* const name, string& spriteFilepath) noexcept
{
	if (ImGui::Button(StringBuilder("Load##", name).CStr()))
	{
		using namespace FileDialogue;

		string path = GetLoadPath(PathDetails("Sprite", "Assets\\", { "*.png", "*.tga", "*.jpg" }), LimitToAssetFolder::True);
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

bool HexEditor::DrawHDRTextureInput(const char* const name, string& hdrTextureFilepath) noexcept
{
	bool modified = false;

	if (ImGui::Button(StringBuilder("Load##", name).CStr()))
	{
		using namespace FileDialogue;

		string path = GetLoadPath(PathDetails("HDR Texture", "Assets\\", { "*.hdr" }), LimitToAssetFolder::True);
		if (path.size() > 0)
		{
			hdrTextureFilepath = path;
			modified = true;
		}
	}

	ImGui::SameLine();

	ImGui::BeginDisabled();
	ImGui::InputText(name, &hdrTextureFilepath, ImGuiInputTextFlags_ReadOnly);
	ImGui::EndDisabled();

	return modified;
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

void HexEditor::DrawTileDropdown(const char* label, TileID& tileID) noexcept
{
	TileData& tileData = TileData::Get(tileID);

	if (ImGui::BeginCombo(label, tileData.name.c_str()))
	{
		if (ImGui::BeginMenu("Trees"))
		{
			vector<TileData>& trees = TileData::Get(HexType::Tree);
			for (char t = 0; t < trees.size(); t++)
			{
				TileID id = TileID(HexType::Tree, t);
				if (ImGui::Selectable(trees[t].name.c_str(), id == tileID))
				{
					tileID = id;
				}
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Flowers"))
		{
			vector<TileData>& flowers = TileData::Get(HexType::Flower);
			for (char t = 0; t < flowers.size(); t++)
			{
				TileID id = TileID(HexType::Flower, t);
				if (ImGui::Selectable(flowers[t].name.c_str(), id == tileID))
				{
					tileID = id;
				}
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Waters"))
		{
			vector<TileData>& waters = TileData::Get(HexType::Water);
			for (char t = 0; t < waters.size(); t++)
			{
				TileID id = TileID(HexType::Water, t);
				if (ImGui::Selectable(waters[t].name.c_str(), id == tileID))
				{
					tileID = id;
				}
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Land"))
		{
			vector<TileData>& lands = TileData::Get(HexType::Land);
			for (char t = 0; t < lands.size(); t++)
			{
				TileID id = TileID(HexType::Land, t);
				if (ImGui::Selectable(lands[t].name.c_str(), id == tileID))
				{
					tileID = id;
				}
			}
			ImGui::EndMenu();
		}

		ImGui::EndCombo();
	}
}

void HexEditor::DrawUISpriteDropdown(const char* label, UISprite*& uiSprite) noexcept
{
	if (ImGui::BeginCombo(label, uiSprite != nullptr ? StringBuilder( "\"", uiSprite->GetName(), "\" ", (void*)uiSprite).CStr() : "\"None\" nullptr"))
	{
		if (ImGui::Selectable("\"None\" nullptr", uiSprite == nullptr))
		{
			uiSprite = nullptr;
		}
		for (UISprite* sprite : UIManager::sprites)
		{
			if (ImGui::Selectable(StringBuilder("\"", sprite->GetName(), "\" ", (void*)sprite).CStr(), uiSprite != nullptr && uiSprite->GetName() == sprite->GetName()))
			{
				uiSprite = sprite;
			}
		}

		ImGui::EndCombo();
	}
}
