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

	mat4 ProjectionViewMatrix = Camera::main->GetPVMatrix();

	// Bind Shader
	ShaderProgram& sp = *material->shaderProgram;
	sp.Bind();

	// Bind Camera Posiiton
	sp.BindUniform("CameraPosition", Camera::main->GetGlobalPosition());

	// Bind Light
	//material->shaderProgram->BindUniform("AmbientColour", LightingManager::ambientLight.colour);
	
	sp.BindUniform("DirectionalLightCount", (int)1);
	sp.BindUniform(StringBuilder("DirectionalLights[",0,"].colour").value.c_str(), LightingManager::directionalLight.colour);
	sp.BindUniform(StringBuilder("DirectionalLights[",0,"].direction").value.c_str(), LightingManager::directionalLight.direction);

	vector<LightObject*> lightObjects = LightingManager::GetClosestLightObjects(GetGlobalPosition(), 4);
	sp.BindUniform("LightObjectCount", (int)lightObjects.size());

	uint currentMapIndex = 0;
	for (int l = 0; l < lightObjects.size(); l++)
	{
		LightObject& lightObject = *lightObjects[l];

		sp.BindUniform(StringBuilder("LightSpaceMatrices[",l,"]").CStr(), glm::inverse(lightObject.GetMatrix()));
		sp.BindUniform(StringBuilder("LightObjects[",l,"].colour").CStr(), lightObject.colour * lightObject.intensity);
		sp.BindUniform(StringBuilder("LightObjects[",l,"].position").CStr(), lightObject.GetGlobalPosition());
		if (lightObjects[l]->angle[1] == 1.0f) // Check for no angle. 1.0 is the cosine of 0 degrees
		{
			sp.BindUniform(StringBuilder("LightObjects[",l,"].direction").CStr(), vec3(0.0f));
		}
		else
		{
			const mat3& lightRotationMatrix = lightObjects[l]->GetRotationMatrix();
			vec3 direction = glm::normalize(vec3(lightRotationMatrix[2]));
			sp.BindUniform(StringBuilder("LightObjects[",l,"].direction").CStr(), direction);
		}
		sp.BindUniform(StringBuilder("LightObjects[",l,"].range").CStr(), lightObject.range);
		sp.BindUniform(StringBuilder("LightObjects[",l,"].angle").CStr(), lightObject.angle);

		sp.BindUniform(StringBuilder("LightObjects[",l,"].shadowMapCount").CStr(), lightObject.shadowMapCount);
		vector<mat4> pvMatrices = lightObject.GetLightPVMatrices();
		for (int m = 0; m < lightObjects[l]->shadowMapCount; m++)
		{
			sp.BindUniform(StringBuilder("LightObjects[",l,"].shadowMapIndices[",m,"]").CStr(), (int)currentMapIndex);
			sp.BindUniform(StringBuilder("ShadowMaps[",currentMapIndex,"]").CStr(), (int)lightObject.shadowMaps[m]);
			sp.BindUniform(StringBuilder("LightSpaceMatrices[",currentMapIndex,"]").CStr(), pvMatrices[m]);
			currentMapIndex++;
		}
	}
	sp.BindUniform("LightSpaceMatrixCount", (int)currentMapIndex);

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

void MeshRenderer::DrawDepth(mat4 PVMatrix)
{
	if (mesh == nullptr) return;

	ShaderProgram* depthProgram = shaderManager->GetProgram("DepthOnly");
	depthProgram->Bind();

	depthProgram->BindUniform("ProjectionViewModel", PVMatrix * GetMatrix());

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