#include "MeshRenderer.h"

#include "Camera.h"
#include "LightingManager.h"

#include "ShaderManager.h"
#include "MaterialManager.h"
#include "TextureManager.h"
#include "MeshManager.h"

#include "GeneralMacros.h"

void MeshRenderer::Draw()
{
	if (material == nullptr) return;
	if (material->shaderProgram == nullptr) return;
	if (mesh == nullptr) return;

	mat4 ProjectionViewMatrix = mainCamera->GetProjectionMatrix((float)screenWidth, (float)screenHeight) * mainCamera->GetViewMatrix();

	// Bind Shader
	material->shaderProgram->Bind();

	// Bind Camera Posiiton
	material->shaderProgram->BindUniform("CameraPosition", mainCamera->GetGlobalPosition());

	// Bind Light
	//material->shaderProgram->BindUniform("AmbientColour", LightingManager::ambientLight.colour);
	
	material->shaderProgram->BindUniform("DirectionalLightCount", (int)1);
	material->shaderProgram->BindUniform(("DirectionalLights[" + std::to_string(0) + "].colour").c_str(), LightingManager::directionalLight.colour);
	material->shaderProgram->BindUniform(("DirectionalLights[" + std::to_string(0) + "].direction").c_str(), LightingManager::directionalLight.direction);

	vector<PointLight> pointLights = LightingManager::GetClosestPointLights(GetGlobalPosition(), 4);
	material->shaderProgram->BindUniform("PointLightCount", (int)0);
	for (int i = 0; i < pointLights.size(); i++)
	{
		material->shaderProgram->BindUniform(("PointLights[" + std::to_string(i) + "].colour").c_str(), pointLights[i].colour);
		material->shaderProgram->BindUniform(("PointLights[" + std::to_string(i) + "].position").c_str(), pointLights[i].position);
		material->shaderProgram->BindUniform(("PointLights[" + std::to_string(i) + "].range").c_str(), pointLights[i].range);
	}

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

			textureManager->GetTexture(filepath.c_str(), Texture::NonLinear)->Bind(0);
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
		else if (material->uniforms[i].name == "RMAOMap")
		{
			string filepath = material->uniforms[i].GetAsFilepath();
			filepath.pop_back();

			if (filepath == "None") filepath = "DefaultRMAO";

			textureManager->GetTexture(filepath.c_str())->Bind(2);
			material->shaderProgram->BindUniform("RMAOMap", 2);

			continue;
		}
	}

	mesh->Draw();
}

const Mesh* MeshRenderer::GetMesh() const
{
	return mesh;
}

void MeshRenderer::SetMesh(const char* filepath)
{
	mesh = meshManager->GetMesh(filepath);
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

	mesh = meshManager->GetMesh("ProceduralCube");
	material = materialManager->GetMaterial("Default");
}