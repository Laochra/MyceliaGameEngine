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

	virtual void Initialise() override;
	virtual void OnDestroy() override;
	virtual void OnRestore() override;
	virtual void Draw() override;
	virtual void DrawDepth(mat4 PVMatrix) override;
	virtual void DrawGUID() override;

	const Mesh* GetMesh() const;
	void SetMesh(const char* filepath);
	const Material* GetMaterial() const;
	void SetMaterial(const char* filepath);

private:
	Mesh* mesh = nullptr;
	Material* material = nullptr;
};