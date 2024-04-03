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

	// Bind Camera Posiiton
	material->shaderProgram->BindUniform("CameraPosition", mainCamera->GetGlobalPosition());

	// Bind Light
	material->shaderProgram->BindUniform("AmbientColour", LightingManager::ambientLight.colour);
	
	material->shaderProgram->BindUniform("DirectionalLightColour", LightingManager::directionalLight.colour);
	material->shaderProgram->BindUniform("DirectionalLightDirection", LightingManager::directionalLight.direction);

	material->shaderProgram->BindUniform("PointLightColour", LightingManager::pointLight.colour);
	material->shaderProgram->BindUniform("PointLightPosition", LightingManager::pointLight.position);
	material->shaderProgram->BindUniform("PointLightRange", LightingManager::pointLight.range);

	// Bind Transform
	material->shaderProgram->BindUniform("ProjectionViewModel", ProjectionViewMatrix * GetMatrix());

	// Bind Transform for Lighting
	material->shaderProgram->BindUniform("ModelMatrix", GetMatrix());
	
	// Bind Colour and Normal Maps
	for (int i = 0; i < material->uniforms.size(); i++)
	{
		if (material->uniforms[i].name == "ColourMap")
		{
			string filepath = material->uniforms[i].GetAsFilepath();
			filepath.pop_back();

			if (filepath == "None") filepath = "DefaultColour";

			textureManager->GetTexture(filepath.c_str())->Bind(0);
			material->shaderProgram->BindUniform("ColourMap", 0);
			
			continue;
		}
		else if (material->uniforms[i].name == "ColourTint")
		{
			vec3 colourTint;
			material->uniforms[i].Get(&colourTint);
			material->shaderProgram->BindUniform("ColourTint", colourTint);

			continue;
		}
		else if (material->uniforms[i].name == "NormalMap")
		{
			string filepath = material->uniforms[i].GetAsFilepath();
			filepath.pop_back();

			if (filepath == "None") filepath = "DefaultNormal";

			textureManager->GetTexture(filepath.c_str())->Bind(1);
			material->shaderProgram->BindUniform("NormalMap", 1);

			continue;
		}
		else if (material->uniforms[i].name == "SpecularMap")
		{
			string filepath = material->uniforms[i].GetAsFilepath();
			filepath.pop_back();

			if (filepath == "None") filepath = "DefaultColour";

			textureManager->GetTexture(filepath.c_str())->Bind(2);
			material->shaderProgram->BindUniform("SpecularMap", 2);

			continue;
		}
		else if (material->uniforms[i].name == "SpecularGlossiness")
		{
			float specularGlossiness;
			material->uniforms[i].Get(&specularGlossiness);
			material->shaderProgram->BindUniform("SpecularGlossiness", specularGlossiness);

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

	material = materialManager->GetMaterial("Default");
}
void MeshRenderer::OnDestroy()
{
	del(mesh);

	GameObject3D::OnDestroy();
}