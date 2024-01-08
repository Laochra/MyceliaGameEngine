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

	ostringstream stream;
	stream << "Shaders\\" + current.programName + ".shaderprogram";

	string oldFilename = current.filePath;
	current.filePath = stream.str();

	ofstream output(current.filePath.c_str());
	output << std::setw(2) << shaderProgram << "\n";

	if (current.filePath == oldFilename)
	{
		shaderManager->ReloadProgram(current.filePath.c_str());
	}
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

	ifstream input(current.filePath.c_str()); assert(input.good());
	
	path filePath = path(current.filePath);

	current.programName = filePath.filename().string();
	int extensionSize = (int)filePath.extension().string().size();
	for (int i = 0; i < extensionSize; i++) {current.programName.pop_back();}

	json shaderProgram;
	try {input >> shaderProgram;}
	catch(parse_error)
	{
		Log({current.filePath, " was corrupt. All fields defaulted to \"None\".\n"}, ConsoleGUI::Warning);
		Save();
		input = ifstream(current.filePath.c_str()); assert(input.good());
		input >> shaderProgram;
	}

	if (shaderProgram.contains("Vertex")) current.vertShader = shaderProgram["Vertex"];
	else {dirty = true; Log({current.filePath, " did not specify a Vertex shader. Defaulted to \"None\".\n"}, ConsoleGUI::Warning);}

	if (shaderProgram.contains("TessEvaluation")) current.teseShader = shaderProgram["TessEvaluation"];
	else {dirty = true; Log({current.filePath, " did not specify a Tess Evaluation shader. Defaulted to \"None\".\n"}, ConsoleGUI::Warning);}

	if (shaderProgram.contains("TessControl")) current.tescShader = shaderProgram["TessControl"];
	else {dirty = true; Log({current.filePath, " did not specify a Tess Control shader. Defaulted to \"None\".\n"}, ConsoleGUI::Warning);}

	if (shaderProgram.contains("Geometry")) current.geomShader = shaderProgram["Geometry"];
	else {dirty = true; Log({current.filePath, " did not specify a Geometry shader. Defaulted to \"None\".\n"}, ConsoleGUI::Warning);}

	if (shaderProgram.contains("Fragment")) current.fragShader = shaderProgram["Fragment"];
	else {dirty = true; Log({current.filePath, " did not specify a Fragment shader. Defaulted to \"None\".\n"}, ConsoleGUI::Warning);}
	
	if (dirty) Save();
}

void ShaderProgramGUI::Draw()
{
	Initialise();

	if (ImGui::BeginCombo("Current File", current.filePath.c_str()))
	{
		if (ImGui::Selectable("New File", current.filePath == "New File"))
		{
			dirty = false;
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

	ImGui::BeginDisabled(!dirty || current.programName.size() == 0);
		if (ImGui::Button("Save"))
		{
			std::remove(current.filePath.c_str());
			Save();
		}
		ImGui::SameLine();
		if (ImGui::Button("Save New"))
		{
			Save();
		}
		ImGui::SameLine();
		if (ImGui::Button("Revert"))
		{
			Reload();
		}
	ImGui::EndDisabled();
	ImGui::BeginDisabled(current.filePath == "New File");
		ImGui::SameLine();
		if (ImGui::Button("Delete"))
		{
			std::remove(current.filePath.c_str());
			dirty = false;
			current = Fields();
		}
	ImGui::EndDisabled();
	if (dirty)
	{
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 0, 255));
		ImGui::Text("Unsaved");
		ImGui::PopStyleColor();
	}

	ImGui::Spacing();

	DrawShaderPopup(VertexStage, vertexShaders, current.vertShader);
	DrawShaderPopup(TessEvaluationStage, tessEvaluationShaders, current.teseShader);
	DrawShaderPopup(TessControlStage, tessEvaluationShaders, current.tescShader);
	DrawShaderPopup(GeometryStage, geometryShaders, current.geomShader);
	DrawShaderPopup(FragmentStage, fragmentShaders, current.fragShader);
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
	if (ImGui::BeginCombo(stageName.c_str(), currentShader.c_str()))
	{
		bool isNone = currentShader == "None";
		if (ImGui::Selectable("None", isNone))
		{
			dirty = true;
			currentShader = "None";
		}
		if (isNone) ImGui::SetItemDefaultFocus();

		for (int i = 0; i < shaderPaths.size(); i++)
		{
			bool isCurrent = shaderPaths[i].string() == currentShader;
			if (ImGui::Selectable(shaderPaths[i].filename().string().c_str(), isCurrent))
			{
				dirty = true;
				currentShader = shaderPaths[i].string();
			}
			if (isCurrent) ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}
	ImGui::EndDisabled();
}