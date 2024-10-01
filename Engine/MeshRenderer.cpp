#include "MeshRenderer.h"

#include "Camera.h"
#include "LightingManager.h"

#include "ShaderManager.h"
#include "MaterialManager.h"
#include "TextureManager.h"
#include "MeshManager.h"

#include "GeneralMacros.h"

#include <string>

void MeshRenderer::SerialiseTo(json& jsonObj) const
{
	GameObject3D::SerialiseTo(jsonObj);
	MeshRenderer::classID;
	if (mesh != nullptr)
	{
		const char* meshFilepath = mesh->GetFilePath();
		jsonObj["Mesh"] = meshFilepath == nullptr ? "None" : mesh->GetFilePath();
	}
	else
	{
		jsonObj["Mesh"] = "None";
	}
	jsonObj["Material"] = material->GetFilePath();
}
void MeshRenderer::DeserialiseFrom(const json& jsonObj, GuidGeneration guidOptions)
{
	GameObject3D::DeserialiseFrom(jsonObj, guidOptions);

	string meshFilePath = jsonObj["Mesh"];
	if (meshFilePath != "None") mesh = meshManager->GetMesh(meshFilePath.c_str());

	string materialFilePath = jsonObj["Material"];
	material = materialManager->GetMaterial(materialFilePath.c_str());
}
void MeshRenderer::UpdateFrom(const json& jsonObj, GuidGeneration guidOptions)
{
	GameObject3D::UpdateFrom(jsonObj, guidOptions);

	string meshFilePath = jsonObj["Mesh"];
	if (meshFilePath == "None") mesh = nullptr;
	else mesh = meshManager->GetMesh(meshFilePath.c_str());

	string materialFilePath = jsonObj["Material"];
	material = materialManager->GetMaterial(materialFilePath.c_str());
}

void MeshRenderer::Draw()
{
	if (material == nullptr) return;
	if (material->shaderProgram == nullptr) return;
	if (mesh == nullptr) return;

	mat4 ProjectionViewMatrix = AppInfo::ActiveCamera()->GetPVMatrix();

	// Bind Shader
	ShaderProgram& sp = *material->shaderProgram;
	sp.Bind();

	// Bind Camera Posiiton
	sp.BindUniform("CameraPosition", AppInfo::ActiveCamera()->GetGlobalPosition());

	// Bind Light
	//material->shaderProgram->BindUniform("AmbientColour", LightingManager::ambientLight.colour);
	
	sp.BindUniform("DirectionalLightCount", (int)1);
	sp.BindUniform("DirectionalLights[0].colour", LightingManager::directionalLight.colour);
	sp.BindUniform("DirectionalLights[0].direction", LightingManager::directionalLight.direction);

	vector<LightObject*> lightObjects = LightingManager::GetClosestLightObjects(GetGlobalPosition(), 4);
	sp.BindUniform("LightObjectCount", (int)lightObjects.size());

	uint currentMapIndex = 0;
	for (int l = 0; l < lightObjects.size(); l++)
	{
		LightObject& lightObject = *lightObjects[l];

		std::string lStr = std::to_string(l);

		sp.BindUniform(("LightSpaceMatrices[" + lStr + "]").c_str(), glm::inverse(lightObject.GetMatrix()));
		sp.BindUniform(("LightObjects[" + lStr + "].colour").c_str(), lightObject.colour * lightObject.intensity);
		sp.BindUniform(("LightObjects[" + lStr + "].position").c_str(), lightObject.GetGlobalPosition());
		if (lightObjects[l]->angle[1] == 1.0f) // Check for no angle. 1.0 is the cosine of 0 degrees
		{
			sp.BindUniform(("LightObjects[" + lStr + "].direction").c_str(), vec3(0.0f));
		}
		else
		{
			const mat3& lightRotationMatrix = lightObjects[l]->GetRotationMatrix();
			vec3 direction = glm::normalize(vec3(lightRotationMatrix[2]));
			sp.BindUniform(("LightObjects[" + lStr + "].direction").c_str(), direction);
		}
		sp.BindUniform(("LightObjects[" + lStr + "].range").c_str(), lightObject.range);
		sp.BindUniform(("LightObjects[" + lStr + "].angle").c_str(), lightObject.angle);

		sp.BindUniform(("LightObjects[" + lStr + "].softShadows").c_str(), int(lightObject.shadowMode == SoftShadows));
		sp.BindUniform(("LightObjects[" + lStr + "].shadowMapCount").c_str(), lightObject.shadowMapCount);
		vector<mat4> pvMatrices = lightObject.GetLightPVMatrices();
		for (int m = 0; m < lightObjects[l]->shadowMapCount; m++)
		{
			sp.BindUniform(("LightObjects[" + lStr + "].shadowMapIndices[" + std::to_string(m) + "]").c_str(), (int)currentMapIndex);
			sp.BindUniform(("LightSpaceMatrices[" + std::to_string(currentMapIndex) + "]").c_str(), pvMatrices[m]);
			currentMapIndex++;
		}
	}
	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_2D_ARRAY, (int)LightingManager::shadowMaps);
	sp.BindUniform("ShadowMaps", 3);
	sp.BindUniform("LightSpaceMatrixCount", (int)currentMapIndex);

	// Bind Transform
	sp.BindUniform("ProjectionViewModel", ProjectionViewMatrix * GetMatrix());

	// Bind Transform for Lighting
	sp.BindUniform("ModelMatrix", GetMatrix());
	
	// Bind Colour and Normal Maps
	bool colourMapBound = false;
	bool colourTintBound = false;
	bool normalMapBound = false;
	bool rmaoMapBound = false;
	bool emissionColourBound = false;
	bool emissionIntensityBound = false;
	for (int i = 0; i < material->uniforms.size(); i++)
	{
		if (material->uniforms[i].name == "ColourMap")
		{
			string filepath = material->uniforms[i].GetAsFilepath();
			filepath.pop_back();

			if (filepath == "None") filepath = "DefaultColour";

			textureManager->GetTexture(filepath.c_str(), true)->Bind(0);
			sp.BindUniform("ColourMap", 0);
			colourMapBound = true;
			continue;
		}
		else if (material->uniforms[i].name == "ColourTint")
		{
			vec3 colourTint;
			material->uniforms[i].Get(&colourTint);
			colourTint = vec3(powf(colourTint.x, 2.2f), powf(colourTint.y, 2.2f), powf(colourTint.z, 2.2f));
			sp.BindUniform("ColourTint", colourTint);
			colourTintBound = true;
			continue;
		}
		else if (material->uniforms[i].name == "NormalMap")
		{
			string filepath = material->uniforms[i].GetAsFilepath();
			filepath.pop_back();

			if (filepath == "None") filepath = "DefaultNormal";

			textureManager->GetTexture(filepath.c_str())->Bind(1);
			sp.BindUniform("NormalMap", 1);
			normalMapBound = true;
			continue;
		}
		else if (material->uniforms[i].name == "RMAOMap")
		{
			string filepath = material->uniforms[i].GetAsFilepath();
			filepath.pop_back();

			if (filepath == "None") filepath = "DefaultRMAO";

			textureManager->GetTexture(filepath.c_str())->Bind(2);
			sp.BindUniform("RMAOMap", 2);
			rmaoMapBound = true;
			continue;
		}
		else if (material->uniforms[i].name == "EmissionColour")
		{
			vec3 emissionColour;
			material->uniforms[i].Get(&emissionColour);
			emissionColour = vec3(powf(emissionColour.x, 2.2f), powf(emissionColour.y, 2.2f), powf(emissionColour.z, 2.2f));
			sp.BindUniform("EmissionColour", emissionColour);
			emissionColourBound = true;
			continue;
		}
		else if (material->uniforms[i].name == "EmissionIntensity")
		{
			float emissionIntensity;
			material->uniforms[i].Get(&emissionIntensity);
			sp.BindUniform("EmissionIntensity", emissionIntensity);
			emissionIntensityBound = true;
			continue;
		}
	}
	
	if (!colourMapBound)
	{
		textureManager->GetTexture("DefaultColour", true)->Bind(0);
		sp.BindUniform("ColourMap", 0);
	}
	if (!colourTintBound) sp.BindUniform("ColourTint", vec3(1.0f, 1.0f, 1.0f));
	if (!normalMapBound)
	{
		textureManager->GetTexture("DefaultNormal")->Bind(1);
		sp.BindUniform("NormalMap", 1);
	}
	if (!rmaoMapBound)
	{
		textureManager->GetTexture("DefaultRMAO")->Bind(2);
		sp.BindUniform("RMAOMap", 2);
	}
	if (!emissionColourBound) sp.BindUniform("EmissionColour", vec3(1.0f, 1.0f, 1.0f));
	if (!emissionIntensityBound) sp.BindUniform("EmissionIntensity", 0.0f);

	if (AppInfo::CompareState(AppState::Editor)) sp.BindUniform("Selected", (int)selected);
	else sp.BindUniform("Selected", (int)false);

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

void MeshRenderer::DrawGUID()
{
	if (mesh == nullptr) return;
	
	ShaderProgram* guidProgram = shaderManager->GetProgram("DrawGUID");
	guidProgram->Bind();

	guidProgram->BindUniform("ProjectionViewModel", AppInfo::ActiveCamera()->GetPVMatrix() * GetMatrix());

	unsigned long long guid = GetGUID();
	guidProgram->BindUniform("GUID", *(glm::uvec2*)&guid);

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

	if (material == nullptr) material = materialManager->GetMaterial("Default");
}

void MeshRenderer::OnDestroy()
{
	GameObject3D::OnDestroy();

	Updater::DrawRemove(this);
}

void MeshRenderer::OnRestore()
{
	GameObject3D::OnRestore();

	Updater::DrawAdd(this);

	if (mesh == nullptr) mesh = meshManager->GetMesh("Cube");
	if (material == nullptr) material = materialManager->GetMaterial("Default");
}
