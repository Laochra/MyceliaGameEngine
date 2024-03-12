#include "MeshRenderer.h"

#include "Camera.h"
#include "LightingManager.h"

#include "ShaderManager.h"

#include "MemoryManagement.h"

void MeshRenderer::Draw()
{
	if (shaderProgram == nullptr) return;

	mat4 ProjectionViewMatrix = mainCamera->GetProjectionMatrix(screenWidth, screenHeight) * mainCamera->GetViewMatrix();

	// Bind Shader
	shaderProgram->Bind();

	// Bind Light
	shaderProgram->BindUniform("AmbientColour", LightingManager::ambientLight);
	shaderProgram->BindUniform("LightColour", LightingManager::light.colour);
	shaderProgram->BindUniform("LightDirection", LightingManager::light.direction);

	// Bind Transform
	shaderProgram->BindUniform("ProjectionViewModel", ProjectionViewMatrix * GetMatrix());

	// Bind Transform for Lighting
	shaderProgram->BindUniform("ModelMatrix", GetMatrix());

	texture->Bind(0);
	shaderProgram->BindUniform("diffuseTex", 0);

	mesh->Draw();
}

Mesh MeshRenderer::GetMesh()
{
	return *mesh;
}

Texture* MeshRenderer::GetTexture()
{
	return texture;
}

void MeshRenderer::Initialise()
{
	GameObject3D::Initialise();

	Updater::DrawAdd(this);

	mesh = new Mesh();
	mesh->InitialiseCube();

	texture = new Texture();
	texture->Load("shroom.png");

	shaderProgram = shaderManager->GetProgram("Assets\\Shaders\\DefaultLit.shaderprogram");
}
void MeshRenderer::OnDestroy()
{
	del(mesh);
	del(texture);

	GameObject3D::OnDestroy();
}