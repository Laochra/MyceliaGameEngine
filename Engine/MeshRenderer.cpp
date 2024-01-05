#include "MeshRenderer.h"

#include "Camera.h"
#include "LightingManager.h"

void MeshRenderer::Draw()
{
	mat4 ProjectionViewMatrix = mainCamera->GetProjectionMatrix(screenWidth, screenHeight) * mainCamera->GetViewMatrix();

	// Bind Shader
	shaderProgram.Bind();

	// Bind Light
	shaderProgram.BindUniform("AmbientColour", LightingManager::ambientLight);
	shaderProgram.BindUniform("LightColour", LightingManager::light.colour);
	shaderProgram.BindUniform("LightDirection", LightingManager::light.direction);

	// Bind Transform
	shaderProgram.BindUniform("ProjectionViewModel", ProjectionViewMatrix * GetMatrix());

	// Bind Transform for Lighting
	shaderProgram.BindUniform("ModelMatrix", GetMatrix());

	texture->Bind(0);
	shaderProgram.BindUniform("diffuseTex", 0);

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
	Updater::DrawAdd(this);

	mesh = new Mesh();
	mesh->InitialiseCube();

	texture = new Texture();
	texture->Load("shroom.png");

	shaderProgram.LoadShader(VertexStage, "Simple.vert");
	shaderProgram.LoadShader(FragmentStage, "Simple.frag");

	if (shaderProgram.Link() == false)
	{
		std::cout << "Shader Error: " << shaderProgram.GetLastError();
		return;
	}
}
void MeshRenderer::OnDestroy()
{
	delete mesh;
	delete texture;
}