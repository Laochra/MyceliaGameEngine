#include "HexTile.h"

#include "Habitat.h"

#include "ShaderManager.h"
#include "Camera.h"

void HexTile::SerialiseTo(json& jsonObj) const
{
	MeshRenderer::SerialiseTo(jsonObj);

	jsonObj["HexType"] = (int)type;
	jsonObj["HexVariant"] = variant;
}
void HexTile::DeserialiseFrom(const json& jsonObj, GuidGeneration guidOptions)
{
	MeshRenderer::DeserialiseFrom(jsonObj, guidOptions);

	type = (HexType)(int)jsonObj["HexType"];
	variant = jsonObj["HexVariant"];
}
void HexTile::UpdateFrom(const json& jsonObj, GuidGeneration guidOptions)
{
	MeshRenderer::UpdateFrom(jsonObj, guidOptions);

	type = (HexType)(int)jsonObj["HexType"];
	variant = jsonObj["HexVariant"];
}


const std::vector<glm::ivec2> HexTile::DirVec =
{
	glm::ivec2(-1,	-1),
	glm::ivec2( 0,	-2),
	glm::ivec2( 1,	-1),
	glm::ivec2( 1,	 1),
	glm::ivec2( 0,	 2),
	glm::ivec2(-1,	 1)
};

static void LoadVariantsOfType(vector<TileData>& type, vector<json> variantsJSON) noexcept
{
	for (const json& variantJSON : variantsJSON)
	{
		TileData variant(((string)variantJSON["Name"]).c_str());

		vector<string> prefabFilepaths = variantJSON["Prefabs"];
		variant.prefabFilepaths[0] = prefabFilepaths[0];
		variant.prefabFilepaths[1] = prefabFilepaths[1];
		variant.prefabFilepaths[2] = prefabFilepaths[2];

		type.push_back(variant);
	}
}
void HexTile::LoadTileDataFrom(json& tilesFile) noexcept
{
	if (!tilesFile.contains("Defaults"))
	{
		tilesFile["Defaults"]["DefaultTile"] = "None";
		tilesFile["Defaults"]["EmptyTile"] = "None";
	}
	HexTile::defaultTilePath = tilesFile["Defaults"]["DefaultTile"];
	HexTile::emptyTilePath = tilesFile["Defaults"]["EmptyTile"];

	if (!tilesFile.contains("Trees")) tilesFile["Trees"] = vector<json>();
	LoadVariantsOfType(HexTile::trees, tilesFile["Trees"]);

	if (!tilesFile.contains("Flowers")) tilesFile["Flowers"] = vector<json>();
	LoadVariantsOfType(HexTile::flowers, tilesFile["Flowers"]);

	if (!tilesFile.contains("Waters")) tilesFile["Waters"] = vector<json>();
	LoadVariantsOfType(HexTile::waters, tilesFile["Waters"]);

	if (!tilesFile.contains("Lands")) tilesFile["Lands"] = vector<json>();
	LoadVariantsOfType(HexTile::lands, tilesFile["Lands"]);

	if (tilesFile.contains("Habitats"))
	{
		vector<json> habitatsJSON = tilesFile["Habitats"];

		for (json habitatJSON : habitatsJSON)
		{
			HabitatData habitat;

			habitat.name = habitatJSON["Name"];

			habitat.prefabFilepath = habitatJSON["PrefabFilepath"];

			vector<json> requiredTiles = habitatJSON["RequiredTiles"];
			habitat.requiredTiles[0] = requiredTiles[0];
			habitat.requiredTiles[1] = requiredTiles[1];
			habitat.requiredTiles[2] = requiredTiles[2];

			Habitat::habitats.push_back(habitat);
		}

		tilesFile["Habitats"] = habitatsJSON;
	}

	if (tilesFile.contains("RadialSprites"))
	{
		json radialSpritesJSON = tilesFile["RadialSprites"];

		treeRadialSprites[0] = radialSpritesJSON["Trees"]["Regular"];
		treeRadialSprites[1] = radialSpritesJSON["Trees"]["Hovered"];
		treeRadialSprites[2] = radialSpritesJSON["Trees"]["Locked"];

		flowerRadialSprites[0] = radialSpritesJSON["Flower"]["Regular"];
		flowerRadialSprites[1] = radialSpritesJSON["Flower"]["Hovered"];
		flowerRadialSprites[2] = radialSpritesJSON["Flower"]["Locked"];

		waterRadialSprites[0] = radialSpritesJSON["Water"]["Regular"];
		waterRadialSprites[1] = radialSpritesJSON["Water"]["Hovered"];
		waterRadialSprites[2] = radialSpritesJSON["Water"]["Locked"];

		landRadialSprites[0] = radialSpritesJSON["Land"]["Regular"];
		landRadialSprites[1] = radialSpritesJSON["Land"]["Hovered"];
		landRadialSprites[2] = radialSpritesJSON["Land"]["Locked"];
	}
}
static void SaveVariantsOfType(json& tilesFile, const char* key, const vector<TileData>& type) noexcept
{
	vector<json> variantsJSON;
	for (const TileData& variant : type)
	{
		json variantJSON;

		variantJSON["Name"] = variant.name;

		vector<string> prefabFilepaths =
		{
			variant.prefabFilepaths[0],
			variant.prefabFilepaths[1],
			variant.prefabFilepaths[2]
		};
		variantJSON["Prefabs"] = prefabFilepaths;

		variantsJSON.push_back(variantJSON);
	}

	tilesFile[key] = variantsJSON;
}
void HexTile::SaveTileDataTo(json& tilesFile) noexcept
{
	tilesFile["Defaults"]["DefaultTile"] = HexTile::defaultTilePath;
	tilesFile["Defaults"]["EmptyTile"] = HexTile::emptyTilePath;

	SaveVariantsOfType(tilesFile, "Trees", HexTile::trees);
	SaveVariantsOfType(tilesFile, "Flowers", HexTile::flowers);
	SaveVariantsOfType(tilesFile, "Waters", HexTile::waters);
	SaveVariantsOfType(tilesFile, "Lands", HexTile::lands);

	vector<json> habitatsJSON;

	for (HabitatData habitat : Habitat::habitats)
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

	tilesFile["Habitats"] = habitatsJSON;

	json radialSpritesJSON;

	radialSpritesJSON["Trees"]["Regular"] = treeRadialSprites[0];
	radialSpritesJSON["Trees"]["Hovered"] = treeRadialSprites[1];
	radialSpritesJSON["Trees"]["Locked"] = treeRadialSprites[2];

	radialSpritesJSON["Flower"]["Regular"] = flowerRadialSprites[0];
	radialSpritesJSON["Flower"]["Hovered"] = flowerRadialSprites[1];
	radialSpritesJSON["Flower"]["Locked"] = flowerRadialSprites[2];

	radialSpritesJSON["Water"]["Regular"] = waterRadialSprites[0];
	radialSpritesJSON["Water"]["Hovered"] = waterRadialSprites[1];
	radialSpritesJSON["Water"]["Locked"] = waterRadialSprites[2];

	radialSpritesJSON["Land"]["Regular"] = landRadialSprites[0];
	radialSpritesJSON["Land"]["Hovered"] = landRadialSprites[1];
	radialSpritesJSON["Land"]["Locked"] = landRadialSprites[2];

	tilesFile["RadialSprites"] = radialSpritesJSON;
}

json HexTile::GetEmptyTilePrefab() noexcept
{
	if (!prefabs.contains("Empty"))
	{
		AddDefaultPrefab("Empty", HexTile::emptyTilePath);
	}
	return prefabs["Empty"];
}
json HexTile::GetDefaultTilePrefab() noexcept
{
	if (!prefabs.contains("Default"))
	{
		AddDefaultPrefab("Default", HexTile::defaultTilePath);
	}
	return prefabs["Default"];
}
json HexTile::GetTilePrefab(string name, uint density) noexcept
{
	string key = StringBuilder(name, density).value;
	if (!prefabs.contains(key))
	{
		AddTilePrefab(name, density);
	}

	return prefabs[key];
}
void HexTile::ClearPrefabs() noexcept
{
	prefabs.clear();
}
void HexTile::AddDefaultPrefab(string name, string path) noexcept
{
	json prefab;

	ifstream prefabFile(path);
	if (!prefabFile.good())
	{
		Debug::LogError(LogID::ERR101, " File not found! ", path, locationinfo);
		assert(false && "Critical game asset wasn't found. Check log file for details.");
	}
	try { prefabFile >> prefab; }
	catch (parse_error)
	{
		Debug::LogError(LogID::ERR101, " File was corrupt! ", path, locationinfo);
		assert(false && "Critical game asset was corrupt. Check log file for details.");
	}
	
	prefabs.insert(std::pair(name, prefab));
}
void HexTile::AddTilePrefab(string name, uint density) noexcept
{
	json prefab;
	string key = StringBuilder(name, density).value;
	
	bool successfulSoFar = true;
	HexType typeFound;

	string path;
	bool foundTile = false;
	for (TileData tree : HexTile::trees)
	{
		if (name == tree.name)
		{
			path = tree.prefabFilepaths[density];
			foundTile = true;
			typeFound = HexType::Tree;
			break;
		}
	}
	if (!foundTile)
	{
		for (TileData flower : HexTile::flowers)
		{
			if (name == flower.name)
			{
				path = flower.prefabFilepaths[density];
				foundTile = true;
				typeFound = HexType::Flower;
				break;
			}
		}
		if (!foundTile)
		{
			for (TileData water : HexTile::waters)
			{
				if (name == water.name)
				{
					path = water.prefabFilepaths[density];
					foundTile = true;
					typeFound = HexType::Water;
					break;
				}
			}
			if (!foundTile)
			{
				for (TileData land : HexTile::lands)
				{
					if (name == land.name)
					{
						path = land.prefabFilepaths[density];
						foundTile = true;
						typeFound = HexType::Land;
						break;
					}
				}
				if (!foundTile)
				{
					Debug::LogWarning(LogID::WRN103, "No tile variant with the name ", name, " was found. It will instead use the default tile file.", locationinfo);
					successfulSoFar = false;
				}
			}
		}
	}

	if (successfulSoFar)
	{
		ifstream prefabFile(path);
		if (path == "None")
		{
			Debug::LogWarning(name, " ", density, " doesn't have a prefab specified. It will instead use the default tile file.", locationinfo);
			successfulSoFar = false;
		}
		else if (!prefabFile.good())
		{
			Debug::LogWarning(LogID::WRN101, name, " ", density, ". It will instead use the default tile file.", locationinfo);
			successfulSoFar = false;
		}
		else
		{
			try { prefabFile >> prefab; }
			catch (parse_error)
			{
				Debug::LogWarning(LogID::WRN102, name, " ", density, ". It will instead use the default tile file.", locationinfo);
				successfulSoFar = false;
			}
		}
	}

	if (successfulSoFar)
	{
		if (prefab["HexType"] != typeFound || prefab["HexVariant"] != name)
		{
			prefab["HexType"] = typeFound;
			prefab["HexVariant"] = name;
			ofstream updatedFile(path);
			updatedFile << prefab;
		}
	}
	else
	{
		prefab = prefabs["Default"];
	}

	prefabs.insert(std::pair(key, prefab));
}

string HexTile::emptyTilePath = "None";
string HexTile::defaultTilePath = "None";
vector<TileData> HexTile::trees;
vector<TileData> HexTile::flowers;
vector<TileData> HexTile::waters;
vector<TileData> HexTile::lands;
std::map<string, json> HexTile::prefabs;

string HexTile::treeRadialSprites[3]   { "None", "None", "None" };
string HexTile::flowerRadialSprites[3] { "None", "None", "None" };
string HexTile::waterRadialSprites[3]  { "None", "None", "None" };
string HexTile::landRadialSprites[3]   { "None", "None", "None" };

void HexTile::Initialise()
{
	MeshRenderer::Initialise();
	SetMesh("Hexagon");
}
void HexTile::DrawHexPos() noexcept
{
	if (habitat != nullptr) return;

	ShaderProgram* hexPosProgram = shaderManager->GetProgram("Assets\\Shaders\\DrawHexPos.gpu");
	hexPosProgram->Bind();

	if (GetMesh() != nullptr)
	{
		hexPosProgram->BindUniform("ProjectionViewModel", AppInfo::ActiveCamera()->GetPVMatrix() * GetMatrix());
		hexPosProgram->BindUniform("HexPos", GetHexPos());
		GetMesh()->Draw();
	}
}

static void DrawFogMasksRecursive(GameObject3D* gameObject, ShaderProgram* fogMaskProgram)
{
	MeshRenderer* meshRenderer = dynamic_cast<MeshRenderer*>(gameObject);
	if (meshRenderer != nullptr && meshRenderer->GetMesh() != nullptr)
	{
		fogMaskProgram->BindUniform("ProjectionViewModel", AppInfo::ActiveCamera()->GetPVMatrix() * meshRenderer->GetMatrix());
		meshRenderer->GetMesh()->Draw();
	}

	const vector<GameObject3D*>* children = gameObject->GetChildren();
	for (vector<GameObject3D*>::const_iterator it = children->begin(); it < children->end(); it++)
	{
		DrawFogMasksRecursive(*it, fogMaskProgram);
	}
}
void HexTile::DrawFogMasks() noexcept
{

}

glm::ivec2 HexTile::GetHexPos() const noexcept
{
	return HexTile::RealPosToHexPos(GetPosition());
}

HexTile*& HexTile::operator[](HexDir direction) noexcept
{
	return adjacent[(unsigned int)direction];
}

vec3 HexTile::HexPosToRealPos(glm::ivec2 hexPos)
{
	return vec3(hexPos.x * 0.866f, 0, hexPos.y * 0.5f);
}
glm::ivec2 HexTile::RealPosToHexPos(vec3 realPos)
{
	return glm::ivec2(std::nearbyintf(realPos.x * (1 / 0.866f)), std::nearbyintf(realPos.z * (1 / 0.5f)));
}
HexDir HexTile::OppositeDir(HexDir dir)
{
	switch (dir)
	{
	case HexDir::NorthWest: return HexDir::SouthEast;
	case HexDir::North:		return HexDir::South;
	case HexDir::NorthEast: return HexDir::SouthWest;
	case HexDir::SouthEast: return HexDir::NorthWest;
	case HexDir::South:		return HexDir::North;
	case HexDir::SouthWest:	return HexDir::NorthEast;
	default:						return (HexDir)0;
	}
}
