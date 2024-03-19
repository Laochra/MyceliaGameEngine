#include "MeshRenderer.h"

#include "Camera.h"
#include "LightingManager.h"

#include "ShaderManager.h"
#include "MaterialManager.h"
#include "TextureManager.h"

#include "MemoryManagement.h"

void MeshRenderer::Draw()
{
	if (material == nullptr) return;
	if (material->shaderProgram == nullptr) return;

	mat4 ProjectionViewMatrix = mainCamera->GetProjectionMatrix(screenWidth, screenHeight) * mainCamera->GetViewMatrix();

	// Bind Shader
	material->shaderProgram->Bind();

	// Bind Light
	material->shaderProgram->BindUniform("AmbientColour", LightingManager::ambientLight);
	material->shaderProgram->BindUniform("LightColour", LightingManager::light.colour);
	material->shaderProgram->BindUniform("LightDirection", LightingManager::light.direction);

	// Bind Transform
	material->shaderProgram->BindUniform("ProjectionViewModel", ProjectionViewMatrix * GetMatrix());

	// Bind Transform for Lighting
	material->shaderProgram->BindUniform("ModelMatrix", GetMatrix());

	//vector<byte> texturePathRaw = material->uniforms[0].GetRaw();
	//string texturePath; for (byte currentChar : texturePathRaw) { texturePath.push_back(currentChar); }
	//Texture* texture = new Texture(texturePath.c_str());
	
	for (int i = 0; i < material->uniforms.size(); i++)
	{
		if (material->uniforms[i].name == "diffuseTex")
		{
			string filepath = material->uniforms[i].GetAsFilepath();
			if (filepath != "None") // TODO: Finish debugging this mess
			{
				textureManager->GetTexture(material->uniforms[i].GetAsFilepath().c_str())->Bind(0);
				material->shaderProgram->BindUniform("diffuseTex", 0);
			}
			
			break;
		}
	}

	mesh->Draw();
}

Mesh MeshRenderer::GetMesh()
{
	return *mesh;
}

Material* MeshRenderer::GetMaterial()
{
	return material;
}

void MeshRenderer::Initialise()
{
	GameObject3D::Initialise();

	Updater::DrawAdd(this);

	mesh = new Mesh();
	mesh->InitialiseCube();

	material = materialManager->GetMaterial("Assets\\Materials\\Mush.mat");
}
void MeshRenderer::OnDestroy()
{
	del(mesh);

	GameObject3D::OnDestroy();
}