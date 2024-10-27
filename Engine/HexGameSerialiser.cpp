#include "HexGameSerialiser.h"

#include "UIManager.h"

#include "HexGameInfo.h"
#include "TileData.h"
#include "HabitatData.h"
#include "SpiritData.h"
#include "HexRadial.h"
#include "HexScrapbook.h"
#include "HexProgression.h"
#include "HexCameraData.h"
#include "HexAudio.h"
#include "HexFog.h"

#include "Debug.h"

void HexGameSerialiser::LoadDataFrom(json& dataFile) noexcept
{
	if (dataFile.contains("Tiles"))
	{
		json tilesJSON = dataFile["Tiles"];

		TileData::defaultTilePaths[0] = tilesJSON["Required"]["Default"];
		TileData::defaultTilePaths[1] = tilesJSON["Required"]["Perimeter"];
		TileData::defaultTilePaths[2] = tilesJSON["Required"]["MotherTree"];

		for (const json& variantJSON : tilesJSON["Trees"])
		{
			TileData variant(((string)variantJSON["Name"]).c_str());
			variant.prefabFilepath = variantJSON["Prefab"];
			TileData::Get(HexType::Tree).push_back(variant);
		}
		for (const json& variantJSON : tilesJSON["Flowers"])
		{
			TileData variant(((string)variantJSON["Name"]).c_str());
			variant.prefabFilepath = variantJSON["Prefab"];
			TileData::Get(HexType::Flower).push_back(variant);
		}
		for (const json& variantJSON : tilesJSON["Waters"])
		{
			TileData variant(((string)variantJSON["Name"]).c_str());
			variant.prefabFilepath = variantJSON["Prefab"];
			TileData::Get(HexType::Water).push_back(variant);
		}
		for (const json& variantJSON : tilesJSON["Lands"])
		{
			TileData variant(((string)variantJSON["Name"]).c_str());
			variant.prefabFilepath = variantJSON["Prefab"];
			TileData::Get(HexType::Land).push_back(variant);
		}
	}

	if (dataFile.contains("Habitats"))
	{
		vector<json> habitatsJSON = dataFile["Habitats"];

		for (json habitatJSON : habitatsJSON)
		{
			HabitatData habitat;

			habitat.name = habitatJSON["Name"];

			habitat.prefabFilepath = habitatJSON["PrefabFilepath"];

			vector<json> requiredTiles = habitatJSON["RequiredTiles"];
			habitat.requiredTiles[0] = requiredTiles[0];
			habitat.requiredTiles[1] = requiredTiles[1];
			habitat.requiredTiles[2] = requiredTiles[2];

			HabitatData::habitatsData.push_back(habitat);
		}
	}

	if (dataFile.contains("Spirits"))
	{
		vector<json> spiritsJSON = dataFile["Spirits"];

		for (json spiritJSON : spiritsJSON)
		{
			SpiritData spirit;
			spirit.name = spiritJSON["Name"];
			spirit.prefabFilepath = spiritJSON["Prefab"];
			SpiritData::spiritsData.push_back(spirit);
		}
	}

	if (dataFile.contains("RadialSprites"))
	{
		json radialSpritesJSON = dataFile["RadialSprites"];

		HexRadial::treeRadialSprites[0] = radialSpritesJSON["Trees"]["Regular"];
		HexRadial::treeRadialSprites[1] = radialSpritesJSON["Trees"]["Hovered"];
		HexRadial::treeRadialSprites[2] = radialSpritesJSON["Trees"]["Locked"];

		HexRadial::flowerRadialSprites[0] = radialSpritesJSON["Flower"]["Regular"];
		HexRadial::flowerRadialSprites[1] = radialSpritesJSON["Flower"]["Hovered"];
		HexRadial::flowerRadialSprites[2] = radialSpritesJSON["Flower"]["Locked"];

		HexRadial::waterRadialSprites[0] = radialSpritesJSON["Water"]["Regular"];
		HexRadial::waterRadialSprites[1] = radialSpritesJSON["Water"]["Hovered"];
		HexRadial::waterRadialSprites[2] = radialSpritesJSON["Water"]["Locked"];

		HexRadial::landRadialSprites[0] = radialSpritesJSON["Land"]["Regular"];
		HexRadial::landRadialSprites[1] = radialSpritesJSON["Land"]["Hovered"];
		HexRadial::landRadialSprites[2] = radialSpritesJSON["Land"]["Locked"];
	}

	if (dataFile.contains("ScrapbookSprites"))
	{
		json scrapbookJSON = dataFile["ScrapbookSprites"];

		vector<json> uiSpritesJSON = scrapbookJSON["UISprites"];
		for (json uiSpriteJSON : uiSpritesJSON)
		{
			UISprite* uiSprite = new UISprite;
			uiSprite->DeserialiseFrom(uiSpriteJSON);
			UIManager::sprites.push_back(uiSprite);
		}

		string baseName = scrapbookJSON["Base"];
		HexScrapbook::base = nullptr;
		if (baseName != "None")
		{
			for (UISprite*& uiSprite : UIManager::sprites)
			{
				if (uiSprite->GetName() == baseName)
				{
					HexScrapbook::base = uiSprite;
					break;
				}
			}
		}
		HexScrapbook::baseTexture = scrapbookJSON["BaseSprite"];


		vector<json> scrapbookHabitatsJSON = scrapbookJSON["Habitats"];

		for (json habitatJSON : scrapbookHabitatsJSON)
		{
			HexScrapbook::HabitatCollection habitat;
			string habitatName = habitatJSON["Habitat"];
			habitat.habitat = nullptr;
			if (habitatName != "None")
			{
				for (UISprite*& uiSprite : UIManager::sprites)
				{
					if (uiSprite->GetName() == habitatName)
					{
						habitat.habitat = uiSprite;
						break;
					}
				}
			}
			habitat.habitatTextures[0] = habitatJSON["HabitatTextures"][0];
			habitat.habitatTextures[1] = habitatJSON["HabitatTextures"][1];

			vector<json> habitatTilesJSON = habitatJSON["Tiles"];
			string tileNames[std::size(habitat.tiles)] = { habitatTilesJSON[0], habitatTilesJSON[1], habitatTilesJSON[2] };
			habitat.tiles[0] = nullptr; habitat.tiles[1] = nullptr; habitat.tiles[2] = nullptr;
			for (UISprite*& uiSprite : UIManager::sprites)
			{
				for (int i = 0; i < (int)habitatTilesJSON.size() && i < std::size(habitat.tiles); i++)
				{
					if (uiSprite->GetName() == tileNames[i])
					{
						habitat.tiles[i] = uiSprite;
					}
				}
			}

			vector<json> habitatTileTexturesJSON = habitatJSON["TileTextures"];
			for (int i = 0; i < (int)habitatTileTexturesJSON.size() && i < std::size(habitat.tileTextures); i++)
			{
				habitat.tileTextures[i] = habitatTileTexturesJSON[i];
			}
			HexScrapbook::habitats.push_back(habitat);
		}
	}

	if (dataFile.contains("StickerEvent"))
	{
		json stickerEventJSON = dataFile["StickerEvent"];
		HexProgression::stickerEvent.moveAmount = stickerEventJSON["MoveAmount"];
		HexProgression::stickerEvent.moveSpeed = stickerEventJSON["MoveSpeed"];

		HexProgression::stickerEvent.startDelay = stickerEventJSON["StartDelay"];
		HexProgression::stickerEvent.stickerPlaceDelay = stickerEventJSON["StickerPlaceDelay"];
		HexProgression::stickerEvent.closeDelay = stickerEventJSON["CloseDelay"];
		HexProgression::stickerEvent.expandBorderDelay = stickerEventJSON["ExpandBorderDelay"];
		HexProgression::stickerEvent.endDelay = stickerEventJSON["EndDelay"];
	}

	if (dataFile.contains("Progression"))
	{
		json progressionData = dataFile["Progression"];
		HexProgression::LoadFrom(progressionData);
	}

	if (dataFile.contains("Fog"))
	{
		json fogJSON = dataFile["Fog"];

		HexFog::gradientRange = fogJSON["Range"];
		HexFog::animationSpeed = fogJSON["Speed"];

		HexFog::fogTextureFilepath = fogJSON["Texture"];
		HexFog::filter = fogJSON["TextureFilter"];
	}

	if (dataFile.contains("Camera"))
	{
		json cameraJSON = dataFile["Camera"];

		HexCameraData::offsetDirection = vec2((float)cameraJSON["OffsetY"], (float)cameraJSON["OffsetZ"]);
		
		HexCameraData::maxZoom = cameraJSON["MaxZoom"];
		HexCameraData::minZoom = cameraJSON["MinZoom"];
		HexCameraData::startZoom = cameraJSON["StartZoom"];
		HexCameraData::currentZoom = HexCameraData::startZoom;
	}

	if (dataFile.contains("Audio"))
	{
		json audioData = dataFile["Audio"];
		HexAudio::LoadFrom(audioData);
	}

	if (dataFile.contains("Misc Colours"))
	{
		json miscColours = dataFile["Misc Colours"];

		if (miscColours.contains("Highlight"))
		{
			vector<float> highlightColour = miscColours["Highlight"];
			HexGameInfo::highlightColour = *(vec4*)&highlightColour.data()[0];
		}
	}

	if (dataFile.contains("BackgroundPrefab"))
	{
		HexGameInfo::backgroundFilepath = dataFile["BackgroundPrefab"];
		HexGameInfo::LoadBackgroundPrefab(HexGameInfo::backgroundFilepath);
	}
}

void HexGameSerialiser::SaveDataTo(json& dataFile) noexcept
{
	json tilesJSON;
	{
		tilesJSON["Required"]["Default"] = TileData::defaultTilePaths[0];
		tilesJSON["Required"]["Perimeter"] = TileData::defaultTilePaths[1];
		tilesJSON["Required"]["MotherTree"] = TileData::defaultTilePaths[2];

		vector<json> treesJSON, flowersJSON, watersJSON, landsJSON;
		for (const TileData& tree : TileData::Get(HexType::Tree))
		{
			json treeJSON;
			treeJSON["Name"] = tree.name;
			treeJSON["Prefab"] = tree.prefabFilepath;
			treesJSON.push_back(treeJSON);
		}
		for (const TileData& flower : TileData::Get(HexType::Flower))
		{
			json flowerJSON;
			flowerJSON["Name"] = flower.name;
			flowerJSON["Prefab"] = flower.prefabFilepath;
			flowersJSON.push_back(flowerJSON);
		}
		for (const TileData& water : TileData::Get(HexType::Water))
		{
			json waterJSON;
			waterJSON["Name"] = water.name;
			waterJSON["Prefab"] = water.prefabFilepath;
			watersJSON.push_back(waterJSON);
		}
		for (const TileData& land : TileData::Get(HexType::Land))
		{
			json landJSON;
			landJSON["Name"] = land.name;
			landJSON["Prefab"] = land.prefabFilepath;
			landsJSON.push_back(landJSON);
		}
		tilesJSON["Trees"] = treesJSON;
		tilesJSON["Flowers"] = flowersJSON;
		tilesJSON["Waters"] = watersJSON;
		tilesJSON["Lands"] = landsJSON;

		dataFile["Tiles"] = tilesJSON;
	}

	vector<json> habitatsJSON;
	{
		for (HabitatData habitat : HabitatData::habitatsData)
		{
			json habitatJSON;

			habitatJSON["Name"] = habitat.name;
			habitatJSON["PrefabFilepath"] = habitat.prefabFilepath;

			vector<json> requiredTiles;
			requiredTiles.push_back(habitat.requiredTiles[0]);
			requiredTiles.push_back(habitat.requiredTiles[1]);
			requiredTiles.push_back(habitat.requiredTiles[2]);
			habitatJSON["RequiredTiles"] = requiredTiles;

			habitatsJSON.push_back(habitatJSON);
		}
		dataFile["Habitats"] = habitatsJSON;
	}

	vector<json> spiritsJSON;
	{
		for (SpiritData spirit : SpiritData::spiritsData)
		{
			json spiritJSON;
			spiritJSON["Name"] = spirit.name;
			spiritJSON["Prefab"] = spirit.prefabFilepath;
			spiritsJSON.push_back(spiritJSON);
		}
		dataFile["Spirits"] = spiritsJSON;
	}

	json radialSpritesJSON;
	{
		radialSpritesJSON["Trees"]["Regular"] = HexRadial::treeRadialSprites[0];
		radialSpritesJSON["Trees"]["Hovered"] = HexRadial::treeRadialSprites[1];
		radialSpritesJSON["Trees"]["Locked"] = HexRadial::treeRadialSprites[2];

		radialSpritesJSON["Flower"]["Regular"] = HexRadial::flowerRadialSprites[0];
		radialSpritesJSON["Flower"]["Hovered"] = HexRadial::flowerRadialSprites[1];
		radialSpritesJSON["Flower"]["Locked"] = HexRadial::flowerRadialSprites[2];

		radialSpritesJSON["Water"]["Regular"] = HexRadial::waterRadialSprites[0];
		radialSpritesJSON["Water"]["Hovered"] = HexRadial::waterRadialSprites[1];
		radialSpritesJSON["Water"]["Locked"] = HexRadial::waterRadialSprites[2];

		radialSpritesJSON["Land"]["Regular"] = HexRadial::landRadialSprites[0];
		radialSpritesJSON["Land"]["Hovered"] = HexRadial::landRadialSprites[1];
		radialSpritesJSON["Land"]["Locked"] = HexRadial::landRadialSprites[2];

		dataFile["RadialSprites"] = radialSpritesJSON;
	}

	json scrapbookJSON;
	{
		vector<json> uiSpritesJSON;
		for (UISprite* uiSprite : UIManager::sprites)
		{
			if (uiSprite != nullptr)
			{
				json uiSpriteJSON;
				uiSprite->SerialiseTo(uiSpriteJSON);
				uiSpritesJSON.push_back(uiSpriteJSON);
			}
		}
		scrapbookJSON["UISprites"] = uiSpritesJSON;


		if (HexScrapbook::base == nullptr) scrapbookJSON["Base"] = "None";
		else scrapbookJSON["Base"] = HexScrapbook::base->name;

		scrapbookJSON["BaseSprite"] = HexScrapbook::baseTexture;


		vector<json> scrapbookHabitatsJSON;

		for (HexScrapbook::HabitatCollection habitat : HexScrapbook::habitats)
		{
			json habitatJSON;
			if (habitat.habitat == nullptr) habitatJSON["Habitat"] = "None";
			else habitatJSON["Habitat"] = habitat.habitat->name;
			habitatJSON["HabitatTextures"] = habitat.habitatTextures;

			vector<json> habitatTilesJSON;
			for (int i = 0; i < std::size(habitat.tiles); i++)
			{
				if (habitat.tiles[i] == nullptr) habitatTilesJSON.push_back("None");
				else habitatTilesJSON.push_back(habitat.tiles[i]->name);
			}
			habitatJSON["Tiles"] = habitatTilesJSON;
			habitatJSON["TileTextures"] = habitat.tileTextures;
			scrapbookHabitatsJSON.push_back(habitatJSON);
		}

		scrapbookJSON["Habitats"] = scrapbookHabitatsJSON;

		dataFile["ScrapbookSprites"] = scrapbookJSON;
	}

	json stickerEventJSON;
	{
		stickerEventJSON["MoveAmount"] = HexProgression::stickerEvent.moveAmount;
		stickerEventJSON["MoveSpeed"] = HexProgression::stickerEvent.moveSpeed;

		stickerEventJSON["StartDelay"] = HexProgression::stickerEvent.startDelay;
		stickerEventJSON["StickerPlaceDelay"] = HexProgression::stickerEvent.stickerPlaceDelay;
		stickerEventJSON["CloseDelay"] = HexProgression::stickerEvent.closeDelay;
		stickerEventJSON["ExpandBorderDelay"] = HexProgression::stickerEvent.expandBorderDelay;
		stickerEventJSON["EndDelay"] = HexProgression::stickerEvent.endDelay;

		dataFile["StickerEvent"] = stickerEventJSON;
	}

	json progressionJSON;
	{
		HexProgression::SaveTo(progressionJSON);
		dataFile["Progression"] = progressionJSON;
	}

	json fogJSON;
	{
		fogJSON["Range"] = HexFog::gradientRange;
		fogJSON["Speed"] = HexFog::animationSpeed;

		fogJSON["Texture"] = HexFog::fogTextureFilepath;
		fogJSON["TextureFilter"] = HexFog::filter;

		dataFile["Fog"] = fogJSON;
	}

	json cameraJSON;
	{
		cameraJSON["OffsetY"] = HexCameraData::offsetDirection.x;
		cameraJSON["OffsetZ"] = HexCameraData::offsetDirection.y;

		cameraJSON["MaxZoom"] = HexCameraData::maxZoom;
		cameraJSON["MinZoom"] = HexCameraData::minZoom;
		cameraJSON["StartZoom"] = HexCameraData::startZoom;

		dataFile["Camera"] = cameraJSON;
	}

	json audioJSON;
	{
		HexAudio::SaveTo(audioJSON);
		dataFile["Audio"] = audioJSON;
	}

	json miscColours;
	{
		vec4 highlightColour = HexGameInfo::highlightColour;
		miscColours["Highlight"] = vector<float>({
			highlightColour.r,
			highlightColour.g,
			highlightColour.b,
			highlightColour.a
		});

		dataFile["Misc Colours"] = miscColours;
	}

	dataFile["BackgroundPrefab"] = HexGameInfo::backgroundFilepath;
}
