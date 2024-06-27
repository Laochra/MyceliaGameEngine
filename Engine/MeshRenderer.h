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
	void Draw() override;
	void DrawDepth(mat4 PVMatrix) override;

	const Mesh* GetMesh() const;
	void SetMesh(const char* filepath);
	const Material* GetMaterial() const;
	void SetMaterial(const char* filepath);

private:
	Mesh* mesh = nullptr;
	Material* material = nullptr;
};