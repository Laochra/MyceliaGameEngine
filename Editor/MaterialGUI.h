#pragma once

#include "Material.h"
#include "MaterialManager.h"
//#include "ShaderManager.h"

#include "ImGuiIncludes.h"
#include "GUI.h"
#include "JsonIncludes.h"

#include "ConsoleGUI.h"

#include "MathIncludes.h"

#include <sstream>
using std::string;
using std::ostringstream;

namespace MaterialGUI
{
struct Fields
{
	string materialName = "";
	string filePath = "New File";
	string shaderFilePath = "None";

	vector<MaterialInput> attributes = vector<MaterialInput>();
	vector<MaterialInput> uniforms = vector<MaterialInput>();
};

inline vector<path> materials;
inline vector<path> shaderPrograms;

inline Fields current = Fields();
inline bool dirty = false;

inline void Initialise();

inline void Save();
inline void NewLoad(path filePath);
inline void Reload();
inline void Load(string filePathStr);
inline void LoadShaderProgram(string filePathStr);

inline void Draw();
inline void DrawField(MaterialInput& field);
}

void MaterialGUI::Initialise()
{
	materials.clear();
	shaderPrograms.clear();

	string materialsPath = "Assets\\Materials";
	string shadersPath = "Assets\\Shaders";


	path filePath;
	path extension;
	for (const directory_entry& entry : directory_iterator(materialsPath))
	{
		filePath = entry.path();
		extension = filePath.extension();

		if (extension.string() == ".material") materials.push_back(filePath);
	}
	for (const directory_entry& entry : directory_iterator(shadersPath))
	{
		filePath = entry.path();
		extension = filePath.extension();

		if (extension.string() == ".shaderprogram") shaderPrograms.push_back(filePath);
	}
}

void MaterialGUI::Save()
{
	// save out to json file
}
void MaterialGUI::NewLoad(path filePath)
{
	Load(filePath.string());
}
void MaterialGUI::Reload()
{
	Load(current.filePath);
}
void MaterialGUI::Load(string filePathStr)
{
	// apply all data from json file to current cache

	//dirty = false;
	//current = Fields();
	//
	//current.filePath = filePathStr;
	//
	//ifstream materialInput(current.filePath.c_str());
	//assert(materialInput.good());
	//
	//path filePath = path(current.filePath);
	//
	//current.materialName = filePath.filename().string();
	//const int extensionSize = (int)filePath.extension().string().size();
	//for (int i = 0; i < extensionSize; i++) { current.materialName.pop_back(); }
	//
	//json material;
	//try { materialInput >> material; }
	//catch (parse_error)
	//{
	//	Log({ current.filePath, " was corrupt. All fields defaulted to \"None\".\n" }, LogType::Warning);
	//	Save();
	//	materialInput = ifstream(current.filePath.c_str());
	//	assert(materialInput.good());
	//	materialInput >> material;
	//
	//	return;
	//}
	//
	//if (material[""])

	// Field Loading
	//{
	//	if (shaderProgram.contains("Attributes"))
	//	{
	//		vector<json> attributes = shaderProgram["Attributes"];
	//		for (int i = 0; i < current.attributes.size(); i++)
	//		{
	//			if (i >= attributes.size()) { dirty = true; attributes.push_back(json()); }
	//
	//			if (attributes[i].contains("Exposed")) current.attributes[i].exposed = attributes[i]["Exposed"];
	//			else { dirty = true; Log({ "Attribute '", current.attributes[i].name , "' in ", current.filePath, " did not specify an Exposed status.\n" }, LogType::Warning); }
	//		}
	//	}
	//	else { dirty = true; Log({ current.filePath, " did not contain an Attributes list.\n" }, LogType::Warning); }
	//
	//	if (shaderProgram.contains("Uniforms"))
	//	{
	//		vector<json> uniforms = shaderProgram["Uniforms"];
	//		for (int i = 0; i < current.uniforms.size(); i++)
	//		{
	//			if (i >= uniforms.size()) { dirty = true; uniforms.push_back(json()); }
	//
	//			if (uniforms[i].contains("Exposed")) current.uniforms[i].exposed = uniforms[i]["Exposed"];
	//			else { dirty = true; Log({ "Uniform '", current.uniforms[i].name , "' in ", current.filePath, " did not specify an Exposed status.\n" }, LogType::Warning); }
	//		}
	//	}
	//	else { dirty = true; Log({ current.filePath, " did not contain a Uniforms list.\n" }, LogType::Warning); }
	//}

	if (dirty) Save();
}

void MaterialGUI::LoadShaderProgram(string filePathStr)
{
	dirty = true;
	current.shaderFilePath = filePathStr;

	current.attributes.clear();
	current.uniforms.clear();


	ifstream input(current.shaderFilePath.c_str());
	assert(input.good());

	json shaderProgram;
	try { input >> shaderProgram; }
	catch (parse_error)
	{
		Log({ current.shaderFilePath, " was corrupt. All fields defaulted to \"None\".\n" }, LogType::Warning);
		input = ifstream(current.filePath.c_str());
		return;
	}


	// Field Loading
	{
		if (shaderProgram.contains("Attributes"))
		{
			vector<json> attributes = shaderProgram["Attributes"];

			MaterialInput attribute;
			for (int i = 0; i < attributes.size(); i++)
			{
				if (!attributes[i].contains("Exposed"))
				{
					Log({ "Attribute ", std::to_string(i) ," in ", current.shaderFilePath, " did not specify an Exposed status.\n" }, LogType::Warning);
					continue;
				}

				if (attributes[i]["Exposed"] == false) continue;

				if (!attributes[i].contains("Name"))
				{
					Log({ "Attribute ", std::to_string(i) ," in ", current.shaderFilePath, " did not specify a name.\n" }, LogType::Warning);
					continue;
				}

				if (!attributes[i].contains("Type"))
				{
					Log({ "Attribute ", std::to_string(i) ," in ", current.shaderFilePath, " did not specify a type.\n" }, LogType::Warning);
					continue;
				}

				attribute.name = attributes[i]["Name"];
				attribute.type = attributes[i]["Type"];
				current.attributes.push_back(attribute);
			}
		}
		else { Log({ current.shaderFilePath, " did not contain an Attributes list.\n" }, LogType::Warning); }
	
		if (shaderProgram.contains("Uniforms"))
		{
			vector<json> uniforms = shaderProgram["Uniforms"];

			MaterialInput uniform;
			for (int i = 0; i < uniforms.size(); i++)
			{	
				if (!uniforms[i].contains("Exposed"))
				{
					Log({ "Uniform ", std::to_string(i) ," in ",  current.shaderFilePath, " did not specify an Exposed status.\n" }, LogType::Warning);
					continue;
				}

				if (uniforms[i]["Exposed"] == false) continue;

				if (!uniforms[i].contains("Name"))
				{
					Log({ "Uniform ", std::to_string(i) ," in ", current.shaderFilePath, " did not specify a name.\n" }, LogType::Warning);
					continue;
				}

				if (!uniforms[i].contains("Type"))
				{
					Log({ "Uniform ", std::to_string(i) ," in ", current.shaderFilePath, " did not specify a type.\n" }, LogType::Warning);
					continue;
				}
				
				uniform.name = uniforms[i]["Name"];
				uniform.type = uniforms[i]["Type"];
				current.uniforms.push_back(uniform);
			}
		}
		else { dirty = true; Log({ current.shaderFilePath, " did not contain a Uniforms list.\n" }, LogType::Warning); }
	}

}

void MaterialGUI::Draw()
{
	Initialise();

	if (ImGui::BeginCombo("Current File", current.filePath.c_str()))
	{
		if (ImGui::Selectable("New File", current.filePath == "New File"))
		{
			dirty = false;
			current = Fields();
		}

		for (int i = 0; i < materials.size(); i++)
		{
			bool isCurrent = materials[i].string() == current.filePath;
			if (ImGui::Selectable(materials[i].filename().string().c_str(), isCurrent))
			{
				NewLoad(materials[i]);
			}
			if (isCurrent) ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}

	if (ImGui::InputText("Material Name", &current.materialName)) { dirty = true; }

	ImGui::Spacing();

	bool unnamed = current.materialName.size() == 0;
	bool newFile = current.filePath == "New File";

	ImGui::BeginDisabled(!dirty || unnamed);
	{
		if (ImGui::Button("Save"))
		{
			string oldFilePath = current.filePath;
			Save();

			if (current.filePath == oldFilePath) materialManager->ReloadMaterial(current.filePath.c_str());
			else std::remove(oldFilePath.c_str());
		}
		ImGui::SameLine();
		if (ImGui::Button("Save New"))
		{
			string oldFilePath = current.filePath;
			Save();

			if (current.filePath == oldFilePath) materialManager->ReloadMaterial(current.filePath.c_str());
		}
	} ImGui::EndDisabled();

	ImGui::BeginDisabled(!dirty || newFile);
	{
		ImGui::SameLine();
		if (ImGui::Button("Revert")) { Reload(); }
	} ImGui::EndDisabled();

	ImGui::BeginDisabled(newFile);
	{
		ImGui::SameLine();
		if (ImGui::Button("Delete"))
		{
			std::remove(current.filePath.c_str());
			dirty = false;
			current = Fields();
		}
	} ImGui::EndDisabled();
	if (dirty)
	{
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(255, 255, 0, 255), "Unsaved");
	}

	ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

	ImGui::BeginChild("Shader Program and Fields");
	{
		ImGui::SeparatorText("Shader Program");

		if (ImGui::BeginCombo("##Shader Program", current.shaderFilePath.c_str()))
		{
			if (ImGui::Selectable("None", current.shaderFilePath == "None"))
			{
				dirty = true;
				current.shaderFilePath = "None";

				current.attributes.clear();
				current.uniforms.clear();
			}

			for (int i = 0; i < shaderPrograms.size(); i++)
			{
				bool isCurrent = shaderPrograms[i].string() == current.shaderFilePath;
				if (ImGui::Selectable(shaderPrograms[i].filename().string().c_str(), isCurrent))
				{
					LoadShaderProgram(shaderPrograms[i].string());
				}
				if (isCurrent) ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		if (current.attributes.size() != 0) { ImGui::SeparatorText("Attributes"); }
		for (MaterialInput& input : current.attributes)
		{
			ImGui::PushID(input.name.c_str());
			{
				ImGui::Text(input.name.c_str());
				DrawField(input);
			} ImGui::PopID();
			ImGui::Spacing();
		}

		ImGui::Spacing();

		if (current.uniforms.size() != 0) { ImGui::SeparatorText("Uniforms"); }
		for (MaterialInput& input : current.uniforms)
		{
			ImGui::PushID(input.name.c_str());
			{
				ImGui::Text(input.name.c_str());
				DrawField(input);
			} ImGui::PopID();
			ImGui::Spacing();
		}

		ImGui::Spacing(); ImGui::Spacing();

	} ImGui::EndChild();
}

void MaterialGUI::DrawField(MaterialInput& field)
{
	switch (field.type)
	{
		case TextureGL:
		{
			ImGui::Text("Texture goes here");
			break;
		}
		case CubemapGL:
		{
			ImGui::Text("Cubemap goes here");
			break;
		}

		case FloatGL:
		{
			float value = field.Get<float>();
			if (ImGui::DragFloat(field.name.c_str(), &value, 0.01f)) { dirty = true; }
			break;
		}
		case Float2GL:
		{
			vec2 value = field.Get<vec2>();
			if (ImGui::DragFloat2(field.name.c_str(), (float*)&value, 0.01f)) { dirty = true; }
			break;
		}
		case Float3GL:
		{
			vec3 value = field.Get<vec3>();
			if (ImGui::DragFloat3(field.name.c_str(), (float*)&value, 0.01f)) { dirty = true; }
			break;
		}
		case Float4GL:
		{
			vec4 value = field.Get<vec4>();
			if (ImGui::DragFloat4(field.name.c_str(), (float*)&value, 0.01f)) { dirty = true; }
			break;
		}
		
		case IntGL:
		{
			int value = field.Get<int>();
			if (ImGui::DragInt(field.name.c_str(), &value, 1)) { dirty = true; }
			break;
		}
		case Int2GL:
		{
			glm::ivec2 value = field.Get<glm::ivec2>();
			if (ImGui::DragInt2(field.name.c_str(), (int*)&value, 1)) { dirty = true; }
			break;
		}
		case Int3GL:
		{
			glm::ivec3 value = field.Get<glm::ivec3>();
			if (ImGui::DragInt3(field.name.c_str(), (int*)&value, 1)) { dirty = true; }
			break;
		}
		case Int4GL:
		{
			glm::ivec2 value = field.Get<glm::ivec4>();
			if (ImGui::DragInt4(field.name.c_str(), (int*)&value, 1)) { dirty = true; }
			break;
		}


		/// TO-DO: Add remaining field types
	}
}