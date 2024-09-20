#include "HexGameSerialiser.h"

#include "TileData.h"
#include "HabitatData.h"
#include "HexRadial.h"
#include "HexProgression.h"
#include "HexAudio.h"

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

	if (dataFile.contains("Progression"))
	{
		json progressionData = dataFile["Progression"];
		HexProgression::LoadFrom(progressionData);
	}

	if (dataFile.contains("Audio"))
	{
		json audioData = dataFile["Audio"];
		HexAudio::LoadFrom(audioData);
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

	json progressionJSON;
	{
		HexProgression::SaveTo(progressionJSON);
		dataFile["Progression"] = progressionJSON;
	}

	json audioJSON;
	{
		HexAudio::SaveTo(audioJSON);
		dataFile["Audio"] = audioJSON;
	}

}
