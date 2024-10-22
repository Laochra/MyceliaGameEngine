#include "TileData.h"

#include "Debug.h"

std::array<vector<TileData>, 4> TileData::tilesData;
std::array<string, 3> TileData::defaultTilePaths;
std::array<json, 3> TileData::defaultTilePrefabs;

TileData::TileData(const char* const nameInit) noexcept :
	name(nameInit) {}

vector<TileData>& TileData::Get(HexType hexType) noexcept
{
	return tilesData[(size_t)hexType >= 0 ? (size_t)hexType : 0];
}
TileData& TileData::Get(HexType hexType, char variant) noexcept
{
	return Get(hexType)[variant];
}

char TileData::GetVariantIndex(HexType hexType, string name) noexcept
{
	vector<TileData> variants = Get(hexType);
	for (char v = 0; v < (char)variants.size(); v++)
	{
		if (variants[v].name == name) return v;
	}

	return 0;
}

const json* TileData::GetDefaultPrefab() noexcept
{
	if (defaultTilePrefabs[0].empty()) PopulateDefaultPrefab(0);
	return &defaultTilePrefabs[0];
}
const json* TileData::GetPerimeterPrefab() noexcept
{
	if (defaultTilePrefabs[1].empty()) PopulateDefaultPrefab(1);
	return &defaultTilePrefabs[1];
}
const json* TileData::GetMotherTreePrefab() noexcept
{
	if (defaultTilePrefabs[2].empty()) PopulateDefaultPrefab(2);
	return &defaultTilePrefabs[2];
}

const json* TileData::GetPrefab(string name) noexcept
{
	for (char t = 0; t < (char)tilesData.size(); t++)
	{
		vector<TileData>& tileType = tilesData[t];
		for (char v = 0; v < (char)tileType.size(); v++)
		{
			TileData& tileVariant = tileType[v];
			if (name == tileVariant.name)
			{
				if (!tileVariant.prefabLoaded) PopulatePrefab((HexType)t, v);
				return &tileVariant.prefab;
			}
		}
	}
	
	return nullptr;
}

void TileData::ClearAndReset() noexcept
{
	for (json& tilePrefab : defaultTilePrefabs)
	{
		tilePrefab.clear();
	}

	for (char t = 0; t < (char)tilesData.size(); t++)
	{
		vector<TileData>& tileType = tilesData[t];
		for (char v = 0; v < (char)tileType.size(); v++)
		{
			TileData& tileVariant = tileType[v];
			tileVariant.countPlaced = 0U;
			tileVariant.unlocked = 0U;
			tileVariant.prefabLoaded = false;
			tileVariant.prefab.clear();
		}
	}
}

#include "MaterialManager.h"
static void RecursiveLoadMaterial(json& jsonObj) noexcept
{
	if (jsonObj.contains("Material"))
	{
		materialManager->GetMaterial(string(jsonObj["Material"]).c_str());
	}

	vector<json> children = jsonObj["Children"];
	for (int i = 0; i < children.size(); i++)
	{
		RecursiveLoadMaterial(children[i]);
	}
}

void TileData::PopulatePrefab(HexType hexType, char variant) noexcept
{
	TileData& tileData = Get(hexType, variant);
	tileData.prefabLoaded = true;
	if (tileData.prefabFilepath == "None")
	{
		Debug::LogWarning(tileData.name, " doesn't have a prefab specified. It will instead use the default tile file.", locationinfo);
		tileData.prefab = *GetDefaultPrefab();
		return;
	}

	ifstream prefabFile(tileData.prefabFilepath);

	if (!prefabFile.good())
	{
		Debug::LogWarning(LogID::WRN101, tileData.name, ". It will instead use the default tile file.", locationinfo);
		tileData.prefab = *GetDefaultPrefab();
		return;
	}

	try { prefabFile >> tileData.prefab; }
	catch (parse_error)
	{
		Debug::LogWarning(LogID::WRN102, tileData.name, ". It will instead use the default tile file.", locationinfo);
	}

	if (tileData.prefab["HexType"] != hexType || tileData.prefab["HexVariant"] != tileData.name)
	{
		tileData.prefab["HexType"] = hexType;
		tileData.prefab["HexVariant"] = tileData.name;
		
		ofstream updatedFile(tileData.prefabFilepath);
		updatedFile << std::setw(2) << tileData.prefab;
	}

	RecursiveLoadMaterial(tileData.prefab);
}
void TileData::PopulateDefaultPrefab(int index) noexcept
{
	string& prefabFilepath = defaultTilePaths[index];
	json& prefab = defaultTilePrefabs[index];

	if (prefabFilepath == "None")
	{
		Debug::LogError(LogID::ERR101, " Tile prefab not defined!", locationinfo);
		assert(false && "Critical game asset wasn't found. Check log file for details.");
		return;
	}

	ifstream prefabFile(prefabFilepath);
	if (!prefabFile.good())
	{
		Debug::LogError(LogID::ERR101, " Tile prefab not found! ", prefabFilepath, locationinfo);
		assert(false && "Critical game asset wasn't found. Check log file for details.");
		return;
	}

	try { prefabFile >> prefab; }
	catch (parse_error)
	{
		Debug::LogError(LogID::ERR101, " Tile prefab was corrupt! ", prefabFilepath, locationinfo);
		assert(false && "Critical game asset was corrupt. Check log file for details.");
		return;
	}

	switch (index)
	{
	case 1:
		prefab["HexType"] = -2;
		prefab["HexVariant"] = "Perimeter";
		break;
	case 2:
		prefab["HexType"] = -3;
		prefab["HexVariant"] = "MotherTree";
		break;
	default:
		break;
	}
}
