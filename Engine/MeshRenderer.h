#pragma once

#include "GameObject3D.h"
#include "Mesh.h"
#include "Material.h"
#include "Shader.h"

class MeshRenderer : public GameObject3D
{
public:
	SerialiseAs(MeshRenderer);

	using GameObject3D::GameObject3D;

	void Initialise() override;
	void OnDestroy() override;
	void Draw() override;

	Mesh GetMesh();
	Material* GetMaterial();

private:
	Mesh* mesh = nullptr;
	Material* material = nullptr;
};