#pragma once

#include "GameObject3D.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"

class MeshRenderer : public GameObject3D
{
public:
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