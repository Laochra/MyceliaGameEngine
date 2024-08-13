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
json HexTile::availablePrefab;
json HexTile::grassPrefab;
json HexTile::waterPrefab;
json HexTile::flowerPrefab;

void HexTile::Initialise()
{
	MeshRenderer::Initialise();
	SetMesh("ProceduralHexagon");
}
void HexTile::Draw()
{
	if (habitat != nullptr) MeshRenderer::Draw();
}
void HexTile::DrawDepth(mat4 pvMatrix)
{
	if (habitat != nullptr) MeshRenderer::DrawDepth(pvMatrix);
}
void HexTile::DrawDebug()
{
	if (habitat != nullptr) MeshRenderer::DrawDebug();
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

	const vector<GameObject3D*>* children = gameObject->GetChildren();
	for (vector<GameObject3D*>::const_iterator it = children->begin(); it < children->end(); it++)
	{
		DrawHexPosRecursive(*it, hexPosProgram, hexPos);
	}
}
void HexTile::DrawHexPos() noexcept
{
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
