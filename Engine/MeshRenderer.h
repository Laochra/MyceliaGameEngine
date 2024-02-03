#pragma once

#include "GameObject3D.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"

class MeshRenderer : public GameObject3D
{
public:
	using GameObject3D::GameObject3D;
	/// <returns>The GameObject type name as a char string</returns>
	virtual const char* GetName() noexcept override { return "MeshRenderer"; }

	void Initialise() override;
	void OnDestroy() override;
	void Draw() override;

	Mesh GetMesh();
	Texture* GetTexture();

private:
	Mesh* mesh = nullptr;
	Texture* texture = nullptr;
	ShaderProgram* shaderProgram = nullptr;
};