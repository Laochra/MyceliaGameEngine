#include "MeshRenderer.h"

#include "Camera.h"
#include "LightingManager.h"

#include "ShaderManager.h"
#include "MaterialManager.h"
#include "TextureManager.h"
#include "MeshManager.h"

#include "GeneralMacros.h"

void MeshRenderer::SerialiseTo(json& jsonObj) const
{
	GameObject3D::SerialiseTo(jsonObj);

	jsonObj["Mesh"] = mesh->GetFilePath();
	jsonObj["Material"] = material->GetFilePath();
}
void MeshRenderer::DeserialiseFrom(const json& jsonObj, GuidGeneration guidOptions)
{
	GameObject3D::DeserialiseFrom(jsonObj, guidOptions);

	string meshFilePath = jsonObj["Mesh"];
	mesh = meshManager->GetMesh(meshFilePath.c_str());
	string materialFilePath = jsonObj["Material"];
	material = materialManager->GetMaterial(materialFilePath.c_str());
}

void MeshRenderer::Draw()
{
	if (material == nullptr) return;
	if (material->shaderProgram == nullptr) return;
	if (mesh == nullptr) return;

	mat4 ProjectionViewMatrix = mainCamera->GetPVMatrix();

	// Bind Shader
	material->shaderProgram->Bind();

	// Bind Camera Posiiton
	material->shaderProgram->BindUniform("CameraPosition", mainCamera->GetGlobalPosition());

	// Bind Light
	//material->shaderProgram->BindUniform("AmbientColour", LightingManager::ambientLight.colour);
	
	material->shaderProgram->BindUniform("DirectionalLightCount", (int)1);
	material->shaderProgram->BindUniform(("DirectionalLights[" + std::to_string(0) + "].colour").c_str(), LightingManager::directionalLight.colour);
	material->shaderProgram->BindUniform(("DirectionalLights[" + std::to_string(0) + "].direction").c_str(), LightingManager::directionalLight.direction);

	vector<LightObject*> lightObjects = LightingManager::GetClosestLightObjects(GetGlobalPosition(), 4);
	material->shaderProgram->BindUniform("LightObjectCount", (int)lightObjects.size());
	for (int i = 0; i < lightObjects.size(); i++)
	{
		material->shaderProgram->BindUniform(("LightObjects[" + std::to_string(i) + "].colour").c_str(), lightObjects[i]->colour * lightObjects[i]->intensity);
		material->shaderProgram->BindUniform(("LightObjects[" + std::to_string(i) + "].position").c_str(), lightObjects[i]->GetGlobalPosition());
		if (lightObjects[i]->angle[1] == 1.0f) // Check for no angle. 1.0 is the cosine of 0 degrees
		{
			material->shaderProgram->BindUniform(("LightObjects[" + std::to_string(i) + "].direction").c_str(), vec3(0.0f));
		}
		else
		{
			const mat4& lightMatrix = lightObjects[i]->GetMatrix();
			vec3 direction = glm::normalize(vec3(lightMatrix[2]));
			material->shaderProgram->BindUniform(("LightObjects[" + std::to_string(i) + "].direction").c_str(), direction);
		}
		material->shaderProgram->BindUniform(("LightObjects[" + std::to_string(i) + "].range").c_str(), lightObjects[i]->range);
		material->shaderProgram->BindUniform(("LightObjects[" + std::to_string(i) + "].angle").c_str(), lightObjects[i]->angle);
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
		else if (material->uniforms[i].name == "EmissionColour")
		{
			vec3 emissionColour;
			material->uniforms[i].Get(&emissionColour);
			material->shaderProgram->BindUniform("EmissionColour", emissionColour);

			continue;
		}
		else if (material->uniforms[i].name == "EmissionIntensity")
		{
			float emissionIntensity;
			material->uniforms[i].Get(&emissionIntensity);
			material->shaderProgram->BindUniform("EmissionIntensity", emissionIntensity);

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

	if (mesh == nullptr) mesh = meshManager->GetMesh("ProceduralCube");
	if (material == nullptr) material = materialManager->GetMaterial("Default");
}