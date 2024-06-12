#include "Material.h"

#include "ShaderManager.h"

#include "TextureManager.h"

#include "Debug.h"

bool Material::LoadFromJSON(const char* filepathInit) noexcept
{
	int size = 1;
	while (filepathInit[size] != '\0') { size++; }
	filepath = new char[size];
	for (int currentChar = 0; currentChar <= size; currentChar++)
	{
		filepath[currentChar] = filepathInit[currentChar];
	}

	ifstream materialInput(filepathInit);
	if (!materialInput.good())
	{
		debug->Log({ "Failed to find a Material at: ", filepathInit, ". Set to Missing.mat" locationinfo }, Debug::Warning, Debug::WRN101);
		return false;
	}

	attributes.clear();
	uniforms.clear();
	json material;
	try { materialInput >> material; }
	catch (parse_error)
	{
		debug->Log({ filepathInit, " was corrupt. Set to Missing.mat" locationinfo }, Debug::Warning, Debug::WRN102);
		return false;
	}

	// Ensuring Shader Program is Valid
	if (!material.contains("ShaderProgram"))
	{
		material["ShaderProgram"] = "PBRLit";
		return true;
	}

	string shaderFilePath = material["ShaderProgram"];

	ifstream shaderInput(shaderFilePath.c_str());
	if (shaderFilePath == "PBRLit")
	{
		shaderInput = ifstream("Engine\\DefaultAssets\\PBRLit.gpu");
	}
	else if (shaderFilePath == "Unlit")
	{
		shaderInput = ifstream("Engine\\DefaultAssets\\Unlit.gpu");
	}
	else
	{
		shaderInput = ifstream(shaderFilePath.c_str());
	}

	if (!shaderInput.good())
	{
		debug->Log({ "Failed to find ShaderProgram at: ", shaderFilePath, ". Set to \"PBRLit\"" locationinfo }, Debug::Warning, Debug::WRN101);
		material["ShaderProgram"] = "PBRLit";
		return true;
	}

	shaderProgram = shaderManager->GetProgram(shaderFilePath.c_str());

	json shaderProgramJSON;
	try { shaderInput >> shaderProgramJSON; }
	catch (parse_error)
	{
		debug->Log({ shaderFilePath, " was corrupt. Set to \"PBRLit\"" locationinfo }, Debug::Warning, Debug::WRN102);
		shaderFilePath = "PBRLit";
		return true;
	}

	vector<json> attributesJSON = material["Attributes"];
	if (attributesJSON.size() > 0)
	{
		vector<json> shaderAttributes = shaderProgramJSON["Attributes"];

		for (json attribute : attributesJSON)
		{
			for (json shaderAttribute : shaderAttributes)
			{
				if (!shaderAttribute["Exposed"]) continue;

				if (shaderAttribute["Name"] == attribute["Name"])
				{
					string name = attribute["Name"];
					string typeName = shaderAttribute["Type"];
					uint type = GetShaderInputTypeFromName(typeName);

					if (type == TextureGL) // Probably also want this for CubeMap
					{
						string path = attribute["Data"];
						attributes.push_back(MaterialInput(name, path, (ShaderInputType)type));
						Texture* texture = textureManager->GetTexture(path.c_str());
						boundTextureHandles.insert(std::pair<string, uint>(name, texture->glHandle));
					}
					else
					{
						vector<byte> data = attribute["Data"];
						attributes.push_back(MaterialInput(name, data, (ShaderInputType)type));
					}
					break;
				}
			}
		}
	}

	vector<json> uniformsJSON = material["Uniforms"];
	if (uniformsJSON.size() > 0)
	{
		vector<json> shaderUniforms = shaderProgramJSON["Uniforms"];

		for (json uniform : uniformsJSON)
		{
			for (json shaderUniform : shaderUniforms)
			{
				if (shaderUniform["Name"] == uniform["Name"])
				{
					string name = uniform["Name"];
					string typeName = shaderUniform["Type"];
					uint type = GetShaderInputTypeFromName(typeName);

					if (type == TextureGL) // Probably also want this for CubeMap
					{
						string path = uniform["Data"];
						uniforms.push_back(MaterialInput(name, path, (ShaderInputType)type));
					}
					else
					{
						vector<byte> data = uniform["Data"];
						uniforms.push_back(MaterialInput(name, data, (ShaderInputType)type));
					}
					break;
				}
			}
		}
	}

   return true;
}

Material::~Material() noexcept
{
	if (filepath != nullptr &&
		 strcmp(filepath, "Engine\\DefaultAssets\\Default.mat") != 0 &&
		 strcmp(filepath, "Engine\\DefaultAssets\\Missing.mat") != 0)
		{
			delete filepath;
		}
}

const char* Material::GetFilePath() const noexcept
{
	return filepath;
}