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


const std::vector<vec3> HexTile::DirVec =
{
	vec3(-0.866f,	0, -0.5f),
	vec3( 0,			0, -1   ),
	vec3( 0.866f,	0, -0.5f),
	vec3( 0.866f,	0,  0.5f),
	vec3( 0,			0,  1   ),
	vec3(-0.866f,	0,  0.5f)
};
json HexTile::availablePrefab;
json HexTile::grassPrefab;
json HexTile::waterPrefab;

void HexTile::Initialise()
{
	MeshRenderer::Initialise();
	SetMesh("ProceduralHexagon");
}
static void DrawHexPosRecursive(GameObject3D* gameObject, ShaderProgram* hexPosProgram, vec3 hexPos)
{
	MeshRenderer* meshRenderer = dynamic_cast<MeshRenderer*>(gameObject);
	if (meshRenderer != nullptr && meshRenderer->GetMesh() != nullptr)
	{
		hexPosProgram->BindUniform("ProjectionViewModel", Camera::main->GetPVMatrix() * meshRenderer->GetMatrix());
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

	DrawHexPosRecursive(this, hexPosProgram, GetPosition());
}

HexTile*& HexTile::operator[](HexDir direction) noexcept
{
	return adjacent[(unsigned int)direction];
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
