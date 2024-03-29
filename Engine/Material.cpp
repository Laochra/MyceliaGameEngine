#include "Material.h"

#include "ShaderManager.h"

#include "TextureManager.h"

bool Material::LoadFromJSON(const char* filepathInit)
{
   //if (!JSONFileIsValid(filename)) return false;

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
		std::cout << "Failed to find material with the filepath: " << filepathInit << " - Defaulted to Missing.mat\n";
		return false;
	}

	attributes.clear();
	uniforms.clear();
	json material;
	try { materialInput >> material; }
	catch (parse_error)
	{
		//Log({ current.filePath, " was corrupt. All fields defaulted to \"None\".\n" }, LogType::Warning);
		std::cout << filepathInit << " was corrupt. Defaulted to Missing.mat\n";

		return false;
	}

	// Ensuring Shader Program is Valid
	if (!material.contains("ShaderProgram"))
	{
		//Log({ current.filePath, " did not specify a Shader Program. Defaulted to \"None\".\n" }, LogType::Warning);
		material["ShaderProgram"] = "None";
		return true;
	}
	if (material["ShaderProgram"] == "None")
	{
		return true;
	}

	string shaderFilePath = material["ShaderProgram"];

	ifstream shaderInput(shaderFilePath.c_str());
	if (shaderFilePath != "Default") shaderInput = ifstream(shaderFilePath.c_str());
	else shaderInput = ifstream("Engine\\DefaultAssets\\Default.gpu");

	if (!shaderInput.good())
	{
		std::cout << "Failed to find shader program with the filepath: " << shaderFilePath << " - Defaulted to \"None\"\n";
		material["ShaderProgram"] = "None";
		return true;
	}

	if (shaderFilePath != "Default") shaderProgram = shaderManager->GetProgram(shaderFilePath.c_str());
	else shaderProgram = shaderManager->GetProgram("Engine\\DefaultAssets\\Default.gpu");

	json shaderProgramJSON;
	try { shaderInput >> shaderProgramJSON; }
	catch (parse_error)
	{
		std::cout << shaderFilePath << " was corrupt. Set to \"Default\"\n";
		shaderFilePath = "Default";

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
					byte type = shaderAttribute["Type"];

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
					byte type = shaderUniform["Type"];

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

const char* Material::GetFilePath() const
{
	return filepath;
}