#include "MaterialGUI.h"

#include "MaterialManager.h"

#include <sstream>
using std::ostringstream;

#include "MathIncludes.h"

#include "JsonIncludes.h"

namespace MaterialGUI
{
	const string materialsPath = "Assets\\Materials";
	const string shadersPath = "Assets\\Shaders";
	const string texturesPath = "Assets\\Textures";

	vector<path> materials;
	vector<path> shaderPrograms;
	vector<path> textures;

	Fields current = Fields();
	bool dirty = false;

	void MaterialGUI::Initialise()
	{
		materials.clear();
		shaderPrograms.clear();
		textures.clear();

		path filePath;
		string extension;
		for (const directory_entry& entry : directory_iterator(materialsPath))
		{
			filePath = entry.path();
			extension = filePath.extension().string();

			if (extension == ".mat") materials.push_back(filePath);
		}
		for (const directory_entry& entry : directory_iterator(shadersPath))
		{
			filePath = entry.path();
			extension = filePath.extension().string();

			if (extension == ".gpu") shaderPrograms.push_back(filePath);
		}
		for (const directory_entry& entry : directory_iterator(texturesPath))
		{
			filePath = entry.path();
			extension = filePath.extension().string();

			if (extension == ".png" || extension == ".tga" || extension == ".jpg") textures.push_back(filePath); // TODO: Refactor this to allow for more image formats
		}
	}

	void MaterialGUI::Save()
	{
		dirty = false;
		json material;

		material["ShaderProgram"] = current.shaderFilePath;

		vector<json> attributes;
		for (MaterialInput input : current.attributes)
		{
			json attribute;
			attribute["Name"] = input.name;
			if (input.type == ShaderInputType::TextureGL)
			{
				vector<byte> bytes = input.GetRaw();
				if (bytes.size() > 0)
				{
					string data;
					for (int i = 0; i < input.GetByteCount(); i++) { data.push_back(bytes[i]); }
					data.pop_back();
					attribute["Data"] = data;
				}
				else
				{
					attribute["Data"] = "None";
				}
			}
			else attribute["Data"] = input.GetRaw();

			attributes.push_back(attribute);
		}
		material["Attributes"] = attributes;

		vector<json> uniforms;
		for (MaterialInput input : current.uniforms)
		{
			json uniform;
			uniform["Name"] = input.name;
			if (input.type == ShaderInputType::TextureGL)
			{
				vector<byte> bytes = input.GetRaw();
				if (bytes.size() > 0)
				{
					string data;
					for (int i = 0; i < input.GetByteCount(); i++) { data.push_back(bytes[i]); }
					data.pop_back();
					uniform["Data"] = data;
				}
				else
				{
					uniform["Data"] = "None";
				}
			}
			else uniform["Data"] = input.GetRaw();

			uniforms.push_back(uniform);
		}
		material["Uniforms"] = uniforms;

		ostringstream stream;
		stream << materialsPath << '\\' << current.materialName + ".mat";

		current.filePath = stream.str();

		ofstream output(current.filePath.c_str());
		output << std::setw(2) << material << "\n";
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
		dirty = false;
		current = Fields();

		current.filePath = filePathStr;

		ifstream materialInput(current.filePath.c_str());
		assert(materialInput.good());

		path filePath = path(current.filePath);

		current.materialName = filePath.filename().string();
		const int extensionSize = (int)filePath.extension().string().size();
		for (int i = 0; i < extensionSize; i++) { current.materialName.pop_back(); }

		json material;
		try { materialInput >> material; }
		catch (parse_error)
		{
			Debug::LogWarning(WRN102, current.filePath, " was corrupt. All fields defaulted to \"None\".", locationinfo);
			Save();
			materialInput = ifstream(current.filePath.c_str());
			assert(materialInput.good());
			materialInput >> material;

			return;
		}


		// Ensuring Shader Program is Valid
		if (!material.contains("ShaderProgram"))
		{
			Debug::LogWarning(WRN103, current.filePath, " did not specify a Shader Program. Set to \"PBRLit\".", locationinfo);
			material["ShaderProgram"] = "PBRLit";
			Save();
		}

		current.shaderFilePath = material["ShaderProgram"];

		ifstream shaderInput;
		if (current.shaderFilePath == "PBRLit")
		{
			shaderInput = ifstream("Engine\\DefaultAssets\\PBRLit.gpu");
		}
		else if (filePathStr == "Unlit")
		{
			shaderInput = ifstream("Engine\\DefaultAssets\\Unlit.gpu");
		}
		else
		{
			shaderInput = ifstream(current.shaderFilePath.c_str());
		}
		assert(shaderInput.good());

		json shaderProgram;
		try { shaderInput >> shaderProgram; }
		catch (parse_error)
		{
			Debug::LogWarning(WRN102, current.filePath, " was corrupt. Shader Program set to \"PBRLit\".", locationinfo);
			current.shaderFilePath = "PBRLit";
			Save();

			return;
		}

		vector<json> attributes = material["Attributes"];
		if (attributes.size() > 0)
		{
			vector<json> shaderAttributes = shaderProgram["Attributes"];

			for (json attribute : attributes)
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
							current.uniforms.push_back(MaterialInput(name, path, (ShaderInputType)type));
						}
						else
						{
							vector<byte> data = attribute["Data"];
							current.uniforms.push_back(MaterialInput(name, data, (ShaderInputType)type));
						}
						break;
					}
				}
			}
		}

		vector<json> uniforms = material["Uniforms"];
		if (uniforms.size() > 0)
		{
			vector<json> shaderUniforms = shaderProgram["Uniforms"];

			for (json uniform : uniforms)
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
							current.uniforms.push_back(MaterialInput(name, path, (ShaderInputType)type));
						}
						else
						{
							vector<byte> data = uniform["Data"];
							current.uniforms.push_back(MaterialInput(name, data, (ShaderInputType)type));
						}
						break;
					}
				}
			}
		}

		if (dirty) Save();
	}

	void MaterialGUI::LoadShaderProgram(string filePathStr)
	{
		dirty = true;
		current.shaderFilePath = filePathStr;

		current.attributes.clear();
		current.uniforms.clear();


		ifstream input;
		input = ifstream(current.shaderFilePath.c_str());
		if (filePathStr == "PBRLit")
		{
			input = ifstream("Engine\\DefaultAssets\\PBRLit.gpu");
		}
		else if (filePathStr == "Unlit")
		{
			input = ifstream("Engine\\DefaultAssets\\Unlit.gpu");
		}
		else
		{
			input = ifstream(current.shaderFilePath.c_str());
		}
		assert(input.good());

		json shaderProgram;
		try { input >> shaderProgram; }
		catch (parse_error)
		{
			Debug::LogWarning(WRN102, current.shaderFilePath, " was corrupt. All fields defaulted to \"None\".", locationinfo);
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
						Debug::LogWarning(WRN103, "Attribute ", i ," in ", current.shaderFilePath, " did not specify an Exposed status.", locationinfo);
						continue;
					}

					if (attributes[i]["Exposed"] == false) continue;

					if (!attributes[i].contains("Name"))
					{
						Debug::LogWarning(WRN103, "Attribute ", i ," in ", current.shaderFilePath, " did not specify a name.", locationinfo);
						continue;
					}

					if (!attributes[i].contains("Type"))
					{
						Debug::LogWarning(WRN103, "Attribute ", i ," in ", current.shaderFilePath, " did not specify a type.", locationinfo);
						continue;
					}

					attribute.name = attributes[i]["Name"];
					string typeName = attributes[i]["Type"];
					attribute.type = GetShaderInputTypeFromName(typeName);
					current.attributes.push_back(attribute);
				}
			}
			else { Debug::LogWarning(WRN103, current.shaderFilePath, " did not contain an Attributes list.", locationinfo); }

			if (shaderProgram.contains("Uniforms"))
			{
				vector<json> uniforms = shaderProgram["Uniforms"];

				MaterialInput uniform;
				for (int i = 0; i < uniforms.size(); i++)
				{
					if (!uniforms[i].contains("Exposed"))
					{
						Debug::LogWarning(WRN103, "Uniform ", i ," in ",  current.shaderFilePath, " did not specify an Exposed status.", locationinfo);
						continue;
					}

					if (uniforms[i]["Exposed"] == false) continue;

					if (!uniforms[i].contains("Name"))
					{
						Debug::LogWarning(WRN103, "Uniform ", i ," in ", current.shaderFilePath, " did not specify a name.", locationinfo);
						continue;
					}

					if (!uniforms[i].contains("Type"))
					{
						Debug::LogWarning(WRN103, "Uniform ", i ," in ", current.shaderFilePath, " did not specify a type.", locationinfo);
						continue;
					}

					uniform.name = uniforms[i]["Name"];
					string typeName = uniforms[i]["Type"];
					uniform.type = GetShaderInputTypeFromName(typeName);
					current.uniforms.push_back(uniform);
				}
			}
			else { dirty = true; Debug::LogWarning(WRN103, current.shaderFilePath, " did not contain a Uniforms list.", locationinfo); }
		}

	}

	void MaterialGUI::Draw(const char* const name, bool& open)
	{
		ImGui::Begin(name, &open, ImGuiWindowFlags_UnsavedDocument * dirty);

		Initialise();

		if (ImGui::BeginCombo("Current File", current.filePath.c_str()))
		{
			if (ImGui::Selectable("New File", current.filePath == "New File"))
			{
				current = Fields();
				LoadShaderProgram("PBRLit");
				dirty = false;
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
				if (ImGui::Selectable("PBRLit", current.shaderFilePath == "PBRLit"))
				{
					LoadShaderProgram("PBRLit");
				}
				if (ImGui::Selectable("Unlit", current.shaderFilePath == "Unlit"))
				{
					LoadShaderProgram("Unlit");
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

		ImGui::End();
	}

	void MaterialGUI::DrawField(MaterialInput& field)
	{
		ImGui::PushID(field.name.c_str());

		switch (field.type)
		{
		case TextureGL:
		{
			vector<byte> value = field.GetRaw();

			string valueStr;

			bool changeMade = false;

			if (field.GetByteCount() > 0)
			{
				valueStr = (const char*)value.data();
			}
			else
			{
				valueStr = "None";
				if (current.filePath != "New File") changeMade = true;
			}

			if (ImGui::BeginCombo("##Texture", valueStr.c_str()))
			{
				if (ImGui::Selectable("None", valueStr == "None"))
				{
					changeMade = true;
					value.clear();
					valueStr = "None";
				}

				for (int i = 0; i < textures.size(); i++)
				{
					bool isCurrent = textures[i].string() == valueStr;
					if (ImGui::Selectable(textures[i].filename().string().c_str(), isCurrent))
					{
						value.clear();
						changeMade = true;
						valueStr = textures[i].string();
					}
					if (isCurrent) ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			if (changeMade)
			{
				for (int i = 0; i < valueStr.size(); i += sizeof(byte))
				{
					value.push_back(valueStr[i]);
				}
				value.push_back('\0');

				field.SetRaw(value.data(), (int)value.size()); dirty = true;
			}

			break;
		}
		case CubemapGL:
		{
			ImGui::Text("Cubemap goes here");
			break;
		}

		case FloatGL:
		{
			float value;
			if (!field.Get(&value))
			{
				if (current.filePath != "New File") dirty = true;
				value = 0.0f; field.Set(value);
			}

			if (ImGui::DragFloat("##Float", &value, 0.01f))
			{
				dirty = true; field.Set(value);
			}
			break;
		}
		case Float2GL:
		{
			vec2 value;
			if (!field.Get(&value))
			{
				if (current.filePath != "New File") dirty = true;
				value = vec2(); field.Set(value);
			}

			if (ImGui::DragFloat2("##Float2", (float*)&value, 0.01f))
			{
				dirty = true; field.Set(value);
			}

			break;
		}
		case Float3GL:
		{
			vec3 value;
			if (!field.Get(&value))
			{
				if (current.filePath != "New File") dirty = true;
				value = vec3(1); field.Set(value);
			}

			if (ImGui::ColorEdit3("##RGB", (float*)&value))
			{
				dirty = true; field.Set(value);
			}
			break;
		}
		case Float4GL:
		{
			vec4 value;
			if (!field.Get(&value))
			{
				if (current.filePath != "New File") dirty = true;
				value = vec4(1); field.Set(value);
			}

			if (ImGui::ColorEdit4("##RGBA", (float*)&value))
			{
				dirty = true; field.Set(value);
			}
			break;
		}
		case Float2x2GL:
		{
			glm::mat2x2 value;
			if (!field.Get(&value))
			{
				if (current.filePath != "New File") dirty = true;
				value = glm::mat2x2(); field.Set(value);
			}

			bool modified = false;
			if (ImGui::DragFloat2("##1", (float*)&value[0], 0.01f)) { modified = true; }
			if (ImGui::DragFloat2("##2", (float*)&value[1], 0.01f)) { modified = true; }

			if (modified)
			{
				dirty = true; field.Set(value);
			}
			break;
		}
		case Float3x3GL:
		{
			glm::mat3x3 value;
			if (!field.Get(&value))
			{
				if (current.filePath != "New File") dirty = true;
				value = glm::mat3x3(); field.Set(value);
			}

			bool modified = false;
			if (ImGui::DragFloat3("##1", (float*)&value[0], 0.01f)) { modified = true; }
			if (ImGui::DragFloat3("##2", (float*)&value[1], 0.01f)) { modified = true; }
			if (ImGui::DragFloat3("##3", (float*)&value[2], 0.01f)) { modified = true; }

			if (modified)
			{
				dirty = true; field.Set(value);
			}
			break;
		}
		case Float4x4GL:
		{
			glm::mat4x4 value;
			if (!field.Get(&value))
			{
				if (current.filePath != "New File") dirty = true;
				value = glm::mat4x4(); field.Set(value);
			}

			bool modified = false;
			if (ImGui::DragFloat4("##1", (float*)&value[0], 0.01f)) { modified = true; }
			if (ImGui::DragFloat4("##2", (float*)&value[1], 0.01f)) { modified = true; }
			if (ImGui::DragFloat4("##3", (float*)&value[2], 0.01f)) { modified = true; }
			if (ImGui::DragFloat4("##4", (float*)&value[3], 0.01f)) { modified = true; }

			if (modified)
			{
				dirty = true; field.Set(value);
			}
			break;
		}

		case IntGL:
		{
			int value;
			if (!field.Get(&value))
			{
				if (current.filePath != "New File") dirty = true;
				value = 0; field.Set(value);
			}

			if (ImGui::DragInt("##Int", &value, 1))
			{
				dirty = true; field.Set(value);
			}
			break;
		}
		case Int2GL:
		{
			glm::ivec2 value;
			if (!field.Get(&value))
			{
				if (current.filePath != "New File") dirty = true;
				value = glm::ivec2(); field.Set(value);
			}

			if (ImGui::DragInt2("##Int2", (int*)&value, 1))
			{
				dirty = true; field.Set(value);
			}
			break;
		}
		case Int3GL:
		{
			glm::ivec3 value;
			if (!field.Get(&value))
			{
				if (current.filePath != "New File") dirty = true;
				value = glm::ivec3(); field.Set(value);
			}

			if (ImGui::DragInt3("##Int3", (int*)&value, 1))
			{
				dirty = true; field.Set(value);
			}
			break;
		}
		case Int4GL:
		{
			glm::ivec4 value;
			if (!field.Get(&value))
			{
				if (current.filePath != "New File") dirty = true;
				value = glm::ivec4(); field.Set(value);
			}

			if (ImGui::DragInt4("##Int4", (int*)&value, 1))
			{
				dirty = true; field.Set(value);
			}
			break;
		}


		/// TO-DO: Add remaining field types
		}

		ImGui::PopID();
	}
}