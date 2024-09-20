#include "HexTile.h"

#include "Camera.h"
#include "ShaderManager.h"
#include "GameObject3D.h"
#include "MeshRenderer.h"

void HexTile::DrawHexPos(HexOffsetCoord centre) noexcept
{
	if (habitat >= 0) return;

	ShaderProgram* hexPosProgram = shaderManager->GetProgram("Assets\\Shaders\\DrawHexPos.gpu");
	hexPosProgram->Bind();

	MeshRenderer* meshRenderer = dynamic_cast<MeshRenderer*>(object);
	if (meshRenderer != nullptr && meshRenderer->GetMesh() != nullptr)
	{
		hexPosProgram->BindUniform("ProjectionViewModel", AppInfo::ActiveCamera()->GetPVMatrix() * object->GetMatrix());
		vec3 objectPos = object->GetPosition();
		HexOffsetCoord hexCoord = HexOffsetCoord::GetFromPos(vec2(objectPos.x, objectPos.z), centre);
		glm::ivec2 hexPos(hexCoord.x, hexCoord.y);
		hexPosProgram->BindUniform("HexPos", hexPos);
		meshRenderer->GetMesh()->Draw();
	}
}
