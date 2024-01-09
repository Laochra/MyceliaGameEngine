#pragma once

#include "ImGuiIncludes.h"
#include "imgui_stdlib.h"
#include "GUI.h"
#include "Shader.h"
#include "ShaderManager.h"

#include <vector>
using std::vector;

#include <filesystem>
namespace filesystem = std::filesystem;
using filesystem::directory_entry;
using filesystem::directory_iterator;
using filesystem::path;

#include <sstream>
using std::string;
using std::ostringstream;

#include <fstream>
using std::ofstream;
using std::ifstream;

#include "json.hpp"
using json = nlohmann::json;
using parse_error = nlohmann::json_abi_v3_11_3::detail::parse_error;

#include "ConsoleGUI.h"

namespace ShaderProgramGUI
{
struct Fields
{
	string programName = "";
	string filePath = "New File";
	string vertShader = "None";
	string teseShader = "None";
	string tescShader = "None";
	string geomShader = "None";
	string fragShader = "None";

	vector<ShaderInput> attributes = vector<ShaderInput>();
	vector<ShaderInput> uniforms = vector<ShaderInput>();
};

inline vector<path> shaderPrograms;

inline vector<path> vertexShaders;
inline vector<path> tessEvaluationShaders;
inline vector<path> tessellationControlShaders;
inline vector<path> geometryShaders;
inline vector<path> fragmentShaders;

inline Fields current = Fields();
inline bool dirty = false;

inline void Initialise();

inline void Save();
inline void NewLoad(path filePath);
inline void Reload();
inline void Load(string filePathStr);

inline bool validLinkage = false;
inline void ValidateLinkage();

inline void Draw();
inline void DrawShaderPopup(ShaderStage stage, vector<path> shaderPaths, string& currentShader);
}

void ShaderProgramGUI::Initialise()
{
	shaderPrograms.clear();

	vertexShaders.clear();
	tessEvaluationShaders.clear();
	tessellationControlShaders.clear();
	geometryShaders.clear();
	fragmentShaders.clear();

	string shadersPath = "Shaders";

	for (const directory_entry& entry : directory_iterator(shadersPath))
	{
		path filePath = entry.path();
		path extension = filePath.extension();

		if (extension.string() == ".shaderprogram") shaderPrograms.push_back(filePath);
		else if (extension.string() == ".vert") vertexShaders.push_back(filePath);
		else if (extension.string() == ".tese") tessEvaluationShaders.push_back(filePath);
		else if (extension.string() == ".tesc") tessellationControlShaders.push_back(filePath);
		else if (extension.string() == ".geom") geometryShaders.push_back(filePath);
		else if (extension.string() == ".frag") fragmentShaders.push_back(filePath);
	}
}

void ShaderProgramGUI::Save()
{
	dirty = false;
	json shaderProgram;

	shaderProgram["Vertex"] = current.vertShader;
	shaderProgram["TessEvaluation"] = current.teseShader;
	shaderProgram["TessControl"] = current.tescShader;
	shaderProgram["Geometry"] = current.geomShader;
	shaderProgram["Fragment"] = current.fragShader;

	vector<json> attributes;
	for (ShaderInput input : current.attributes)
	{
		json attribute;
		attribute["Name"] = input.name;
		attribute["Type"] = input.type;
		attribute["Exposed"] = input.exposed;
		attributes.push_back(attribute);
	}
	shaderProgram["Attributes"] = attributes;

	vector<json> uniforms;
	for (ShaderInput input : current.uniforms)
	{
		json uniform;
		uniform["Name"] = input.name;
		uniform["Type"] = input.type;
		uniform["Exposed"] = input.exposed;
		uniforms.push_back(uniform);
	}
	shaderProgram["Uniforms"] = uniforms;


	ostringstream stream;
	stream << "Shaders\\" + current.programName + ".shaderprogram";

	current.filePath = stream.str();

	ofstream output(current.filePath.c_str());
	output << std::setw(2) << shaderProgram << "\n";
}

void ShaderProgramGUI::NewLoad(path filePath)
{
	current.filePath = filePath.string();

	Load(current.filePath);
}

void ShaderProgramGUI::Reload()
{
	Load(current.filePath);
}

void ShaderProgramGUI::Load(string filePathStr)
{
	dirty = false;
	current = Fields();

	current.filePath = filePathStr;

	ifstream input(current.filePath.c_str());
	assert(input.good());
	
	path filePath = path(current.filePath);

	current.programName = filePath.filename().string();
	int extensionSize = (int)filePath.extension().string().size();
	for (int i = 0; i < extensionSize; i++) {current.programName.pop_back();}

	json shaderProgram;
	try {input >> shaderProgram;}
	catch (parse_error)
	{
		Log({current.filePath, " was corrupt. All fields defaulted to \"None\".\n"}, LogType::Warning);
		Save();
		input = ifstream(current.filePath.c_str());
		assert(input.good());
		input >> shaderProgram;
	}

	// Shader Loading
	{
		if (shaderProgram.contains("Vertex"))
		{
			current.vertShader = shaderProgram["Vertex"];
			if (current.vertShader == "")
			{
				current.vertShader = "None";
				Log({ current.filePath, " had it's Vertex shader blank. Defaulted to \"None\".\n" }, LogType::Warning);
			}
		}
		else
		{
			dirty = true;
			Log({ current.filePath, " did not specify a Vertex shader. Defaulted to \"None\".\n" }, LogType::Warning);
		}

		if (shaderProgram.contains("TessEvaluation"))
		{
			current.teseShader = shaderProgram["TessEvaluation"];
			if (current.teseShader == "")
			{
				current.teseShader = "None";
				Log({ current.filePath, " had it's Tess Evaluation shader blank. Defaulted to \"None\".\n" }, LogType::Warning);
			}
		}
		else
		{
			dirty = true;
			Log({ current.filePath, " did not specify a Tess Evaluation shader. Defaulted to \"None\".\n" }, LogType::Warning);
		}

		if (shaderProgram.contains("TessControl"))
		{
			current.tescShader = shaderProgram["TessControl"];
			if (current.tescShader == "")
			{
				current.tescShader = "None";
				Log({ current.filePath, " had it's Tess Control shader blank. Defaulted to \"None\".\n" }, LogType::Warning);
			}
		}
		else
		{
			dirty = true;
			Log({ current.filePath, " did not specify a Tess Control shader. Defaulted to \"None\".\n" }, LogType::Warning);
		}

		if (shaderProgram.contains("Geometry"))
		{
			current.geomShader = shaderProgram["Geometry"];
			if (current.geomShader == "")
			{
				current.geomShader = "None";
				Log({ current.filePath, " had it's Geometry shader blank. Defaulted to \"None\".\n" }, LogType::Warning);
			}
		}
		else
		{
			dirty = true;
			Log({ current.filePath, " did not specify a Geometry shader. Defaulted to \"None\".\n" }, LogType::Warning);
		}

		if (shaderProgram.contains("Fragment"))
		{
			current.fragShader = shaderProgram["Fragment"];
			if (current.fragShader == "")
			{
				current.fragShader = "None";
				Log({ current.filePath, " had it's Fragment shader blank. Defaulted to \"None\".\n" }, LogType::Warning);
			}
		}
		else
		{
			dirty = true;
			Log({ current.filePath, " did not specify a Fragment shader. Defaulted to \"None\".\n" }, LogType::Warning);
		}
	}
	
	if (dirty) Save();
	ValidateLinkage();

	// Field Loading
	{
		if (shaderProgram.contains("Attributes"))
		{
			vector<json> attributes = shaderProgram["Attributes"];
			for (int i = 0; i < current.attributes.size(); i++)
			{
				if (i >= attributes.size()) { dirty = true; attributes.push_back(json()); }

				if (attributes[i].contains("Exposed")) current.attributes[i].exposed = attributes[i]["Exposed"];
				else { dirty = true; Log({ "Attribute '", current.attributes[i].name , "' in ", current.filePath, " did not specify an Exposed status.\n" }, LogType::Warning); }
			}
		}
		else { dirty = true; Log({ current.filePath, " did not contain an Attributes list.\n" }, LogType::Warning); }

		if (shaderProgram.contains("Uniforms"))
		{
			vector<json> uniforms = shaderProgram["Uniforms"];
			for (int i = 0; i < current.uniforms.size(); i++)
			{
				if (i >= uniforms.size()) { dirty = true; uniforms.push_back(json()); }

				if (uniforms[i].contains("Exposed")) current.uniforms[i].exposed = uniforms[i]["Exposed"];
				else { dirty = true; Log({ "Uniform '", current.uniforms[i].name , "' in ", current.filePath, " did not specify an Exposed status.\n" }, LogType::Warning); }
			}
		}
		else { dirty = true; Log({ current.filePath, " did not contain a Uniforms list.\n" }, LogType::Warning); }
	}

	if (dirty) Save();
}

void ShaderProgramGUI::ValidateLinkage()
{
	ShaderProgram* testShaderProgram = new ShaderProgram();

	if (current.vertShader != "None") testShaderProgram->LoadShader(VertexStage, current.vertShader.c_str());
	if (current.teseShader != "None") testShaderProgram->LoadShader(TessEvaluationStage, current.teseShader.c_str());
	if (current.tescShader != "None") testShaderProgram->LoadShader(TessControlStage, current.tescShader.c_str());
	if (current.geomShader != "None") testShaderProgram->LoadShader(GeometryStage, current.geomShader.c_str());
	if (current.fragShader != "None") testShaderProgram->LoadShader(FragmentStage, current.fragShader.c_str());

	validLinkage = testShaderProgram->Link();

	testShaderProgram->GetFields(current.attributes, current.uniforms);

	delete testShaderProgram;
}

void ShaderProgramGUI::Draw()
{
	Initialise();

	if (ImGui::BeginCombo("Current File", current.filePath.c_str()))
	{
		if (ImGui::Selectable("New File", current.filePath == "New File"))
		{
			dirty = false;
			validLinkage = false;
			current = Fields();
		}

		for (int i = 0; i < shaderPrograms.size(); i++)
		{
			bool isCurrent = shaderPrograms[i].string() == current.filePath;
			if (ImGui::Selectable(shaderPrograms[i].filename().string().c_str(), isCurrent))
			{
				NewLoad(shaderPrograms[i]);
			}
			if (isCurrent) ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}

	if (ImGui::InputText("Program Name", &current.programName))
	{
		dirty = true;
	}

	ImGui::Spacing();

	bool unnamed = current.programName.size() == 0;
	bool newFile = current.filePath == "New File";

	ImGui::BeginDisabled(!dirty || !validLinkage || unnamed);
	{
		if (ImGui::Button("Save"))
		{
			string oldFilePath = current.filePath;
			Save();

			if (current.filePath == oldFilePath) shaderManager->ReloadProgram(current.filePath.c_str());
			else std::remove(oldFilePath.c_str());
		}
		ImGui::SameLine();
		if (ImGui::Button("Save New"))
		{
			string oldFilePath = current.filePath;
			Save();

			if (current.filePath == oldFilePath) shaderManager->ReloadProgram(current.filePath.c_str());
		}
	} ImGui::EndDisabled();

	ImGui::BeginDisabled(!dirty || newFile);
	{
		ImGui::SameLine();
		if (ImGui::Button("Revert"))
		{
			Reload();
		}
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

	ImGui::BeginChild("Shaders and Fields");
	{
		ImGui::SeparatorText("Shaders");

		DrawShaderPopup(VertexStage, vertexShaders, current.vertShader);
		DrawShaderPopup(TessEvaluationStage, tessEvaluationShaders, current.teseShader);
		DrawShaderPopup(TessControlStage, tessEvaluationShaders, current.tescShader);
		DrawShaderPopup(GeometryStage, geometryShaders, current.geomShader);
		DrawShaderPopup(FragmentStage, fragmentShaders, current.fragShader);

		if (!validLinkage)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 0, 255));
			ImGui::TextWrapped("The current combination cannot be linked");
			ImGui::PopStyleColor();
		}
		else
		{
			ImGui::SeparatorText("Attributes");

			ImVec4 typeColour = ImVec4(0.75f, 0.4f, 0.95f, 1);
			for (ShaderInput& input : current.attributes)
			{
				ImGui::PushID(input.name.c_str());
				{
					if (ImGui::Checkbox("##", &input.exposed))
					{
						dirty = true;
					}
					ImGui::SameLine();
					ImGui::TextColored(typeColour, GetShaderInputTypeName(input.type));
					ImGui::SameLine();
					ImGui::Text(input.name.c_str());
				} ImGui::PopID();
			}
			ImGui::SeparatorText("Uniforms");
			for (ShaderInput& input : current.uniforms)
			{
				ImGui::PushID(input.name.c_str());
				{
					if (ImGui::Checkbox("##", &input.exposed))
					{
						dirty = true;
					}
					ImGui::SameLine();
					ImGui::TextColored(typeColour, GetShaderInputTypeName(input.type));
					ImGui::SameLine();
					ImGui::Text(input.name.c_str());
				}
				ImGui::PopID();
			}
		}
	} ImGui::EndChild();
}

void ShaderProgramGUI::DrawShaderPopup(ShaderStage stage, vector<path> shaderPaths, string& currentShader)
{
	string stageName;
	switch (stage)
	{
	case VertexStage: stageName = "Vertex"; break;
	case TessEvaluationStage: stageName = "Tess Evaluation"; break;
	case TessControlStage: stageName = "Tess Control"; break;
	case GeometryStage: stageName = "Geometry"; break;
	case FragmentStage: stageName = "Fragment"; break;
	}

	ImGui::BeginDisabled(shaderPaths.size() == 0);
	{
		if (ImGui::BeginCombo(stageName.c_str(), currentShader.c_str()))
		{
			bool isNone = currentShader == "None";
			if (ImGui::Selectable("None", isNone))
			{
				dirty = true;
				currentShader = "None";
				ValidateLinkage();
			}
			if (isNone) ImGui::SetItemDefaultFocus();

			for (int i = 0; i < shaderPaths.size(); i++)
			{
				bool isCurrent = shaderPaths[i].string() == currentShader;
				if (ImGui::Selectable(shaderPaths[i].filename().string().c_str(), isCurrent))
				{
					dirty = true;
					currentShader = shaderPaths[i].string();
					ValidateLinkage();
				}
				if (isCurrent) ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}
	} ImGui::EndDisabled();
}