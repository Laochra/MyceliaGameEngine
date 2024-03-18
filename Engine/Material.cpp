#include "Material.h"

#include "ShaderManager.h"

bool Material::LoadFromJSON(const char* filepathInit)
{
   //if (!JSONFileIsValid(filename)) return false;

	ifstream materialInput(filepathInit);
	assert(materialInput.good());

	attributes.clear();
	uniforms.clear();
	json material;
	try { materialInput >> material; }
	catch (parse_error)
	{
		//Log({ current.filePath, " was corrupt. All fields defaulted to \"None\".\n" }, LogType::Warning);

		return false;
	}

	// Ensuring Shader Program is Valid
	if (!material.contains("ShaderProgram"))
	{
		//Log({ current.filePath, " did not specify a Shader Program. Defaulted to \"None\".\n" }, LogType::Warning);
		material["ShaderProgram"] = "None";
	}
	if (material["ShaderProgram"] == "None")
	{
		return false;
	}

	string shaderFilePath = material["ShaderProgram"];

	ifstream shaderInput(shaderFilePath.c_str());
	assert(shaderInput.good());

	shaderProgram = shaderManager->GetProgram(shaderFilePath.c_str());

	json shaderProgramJSON;
	try { shaderInput >> shaderProgramJSON; }
	catch (parse_error)
	{
		//Log({ current.filePath, " was corrupt. Shader Program defaulted to \"None\"." }, LogType::Warning);
		shaderFilePath = "None";

		return false;
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

const char* Material::GetFilePath()
{
	return filepath;
}