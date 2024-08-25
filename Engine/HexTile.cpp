#include "HexTile.h"

#include "ShaderManager.h"
#include "Camera.h"

void HexTile::SerialiseTo(json& jsonObj) const
{
	MeshRenderer::SerialiseTo(jsonObj);

	jsonObj["HexType"] = (int)type;
}
void HexTile::DeserialiseFrom(const json& jsonObj, GuidGeneration guidOptions)
{
	MeshRenderer::DeserialiseFrom(jsonObj, guidOptions);

	type = (HexType)(int)jsonObj["HexType"];
}
void HexTile::UpdateFrom(const json& jsonObj, GuidGeneration guidOptions)
{
	MeshRenderer::UpdateFrom(jsonObj, guidOptions);

	type = (HexType)(int)jsonObj["HexType"];
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
void HexTile::AddDefaultPrefab(string name, string path) noexcept
{
	json prefab;

	ifstream prefabFile(path);
	if (!prefabFile.good())
	{
		Debug::LogError(LogID::ERR101, " File not found! ", path);
		assert(false && "Critical game asset wasn't found. Check log file for details.");
	}
	try { prefabFile >> prefab; }
	catch (parse_error)
	{
		Debug::LogError(LogID::ERR101, " File was corrupt! ", path);
		assert(false && "Critical game asset was corrupt. Check log file for details.");
	}
	
	prefabs.insert(std::pair(name, prefab));
}
void HexTile::AddTilePrefab(string name, uint density) noexcept
{
	json prefab;
	string key = StringBuilder(name, density).value;
	
	bool successfulLoad = true;
	string path;
	// Get path from the name and density, searching through each tile list

	ifstream prefabFile(path);
	if (!prefabFile.good())
	{
		Debug::LogWarning(LogID::WRN101, " It will instead use the default tile file. ", path);
		successfulLoad = false;
	}
	else
	{
		try { prefabFile >> prefab; }
		catch (parse_error)
		{
			Debug::LogWarning(LogID::WRN102, " It will instead use the default tile file. ", path);
			successfulLoad = false;
		}
	}

	if (!successfulLoad)
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

void HexTile::Initialise()
{
	MeshRenderer::Initialise();
	SetMesh("ProceduralHexagon");
}
static void DrawHexPosRecursive(GameObject3D* gameObject, ShaderProgram* hexPosProgram, glm::ivec2 hexPos)
{
	MeshRenderer* meshRenderer = dynamic_cast<MeshRenderer*>(gameObject);
	if (meshRenderer != nullptr && meshRenderer->GetMesh() != nullptr)
	{
		hexPosProgram->BindUniform("ProjectionViewModel", AppInfo::ActiveCamera()->GetPVMatrix() * meshRenderer->GetMatrix());
		hexPosProgram->BindUniform("HexPos", hexPos);
		meshRenderer->GetMesh()->Draw();
	}

	//const vector<GameObject3D*>* children = gameObject->GetChildren();
	//for (vector<GameObject3D*>::const_iterator it = children->begin(); it < children->end(); it++)
	//{
	//	DrawHexPosRecursive(*it, hexPosProgram, hexPos);
	//}
}
void HexTile::DrawHexPos() noexcept
{
	if (habitat != nullptr) return;

	ShaderProgram* hexPosProgram = shaderManager->GetProgram("Assets\\Shaders\\DrawHexPos.gpu");
	hexPosProgram->Bind();

	DrawHexPosRecursive(this, hexPosProgram, GetHexPos());
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
