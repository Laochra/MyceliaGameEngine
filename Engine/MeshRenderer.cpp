#include "MeshRenderer.h"

#include "Camera.h"
#include "LightingManager.h"

#include "ShaderManager.h"
#include "MaterialManager.h"
#include "TextureManager.h"

#include "GeneralMacros.h"

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
	
	for (int i = 0; i < material->uniforms.size(); i++)
	{
		if (material->uniforms[i].name == "ColourMap")
		{
			string filepath = material->uniforms[i].GetAsFilepath();
			filepath.pop_back();
			if (filepath != "None") 
			{
				textureManager->GetTexture(material->uniforms[i].GetAsFilepath().c_str())->Bind(0);
				material->shaderProgram->BindUniform("ColourMap", 0);
			}
			
			continue;
		}
		else if (material->uniforms[i].name == "NormalMap")
		{
			string filepath = material->uniforms[i].GetAsFilepath();
			filepath.pop_back();
			if (filepath != "None")
			{
				textureManager->GetTexture(material->uniforms[i].GetAsFilepath().c_str())->Bind(1);
				material->shaderProgram->BindUniform("NormalMap", 1);
			}

			continue;
		}
	}

	mesh->Draw();
}

const Mesh* MeshRenderer::GetMesh() const
{
	return mesh;
}

const Material* MeshRenderer::GetMaterial() const
{
	return material;
}

void MeshRenderer::SetMaterial(const char* filepath)
{
	material = materialManager->GetMaterial(filepath);
}

void MeshRenderer::Initialise()
{
	GameObject3D::Initialise();

	Updater::DrawAdd(this);

	mesh = new Mesh();
	mesh->LoadFromFile("Assets\\Meshes\\soulspear.obj");

	material = materialManager->GetMaterial("Assets\\Materials\\Mush.mat");
}
void MeshRenderer::OnDestroy()
{
	del(mesh);

	GameObject3D::OnDestroy();
}