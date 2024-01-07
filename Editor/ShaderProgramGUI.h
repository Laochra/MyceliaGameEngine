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

namespace ShaderProgramGUI
{
	inline vector<path> shaderPrograms;

	inline vector<path> vertexShaders;
	inline vector<path> tessellationEvaluationShaders;
	inline vector<path> tessellationControlShaders;
	inline vector<path> geometryShaders;
	inline vector<path> fragmentShaders;

	inline bool dirty = false;

	inline void Initialise();
	inline void Draw();
	inline void DrawShaderPopup(ShaderStage stage, vector<path> shaderPaths, string& currentShader);
}

void ShaderProgramGUI::Initialise()
{
	shaderPrograms.clear();

	vertexShaders.clear();
	tessellationEvaluationShaders.clear();
	tessellationControlShaders.clear();
	geometryShaders.clear();
	fragmentShaders.clear();

	string shadersPath = "Shaders";

	for (const directory_entry& entry : directory_iterator(shadersPath))
	{
		path filePath = entry.path();
		path extension = filePath.extension();

		if (extension.string() == string(".shaderprogram"))
		{
			shaderPrograms.push_back(filePath);
		}
		else if (extension.string() == string(".vert"))
		{
			vertexShaders.push_back(filePath);
		}
		else if (extension.string() == string(".tese"))
		{
			tessellationEvaluationShaders.push_back(filePath);
		}
		else if (extension.string() == string(".tesc"))
		{
			tessellationControlShaders.push_back(filePath);
		}
		else if (extension.string() == string(".geom"))
		{
			geometryShaders.push_back(filePath);
		}
		else if (extension.string() == string(".frag"))
		{
			fragmentShaders.push_back(filePath);
		}
	}
}

void ShaderProgramGUI::Draw()
{
	Initialise();

	static string currentFile = string("New File");

	static string currentVertShader = string("None");
	static string currentTeseShader = string("None");
	static string currentTescShader = string("None");
	static string currentGeomShader = string("None");
	static string currentFragShader = string("None");

	static string programName = string();

	if (ImGui::BeginCombo("Current File", currentFile.c_str()))
	{
		bool isNewFile = currentFile == string("New File");
		if (ImGui::Selectable("New File", isNewFile))
		{
			dirty = false;

			currentFile = string("New File");
			programName = string();
			currentVertShader = string("None");
			currentTeseShader = string("None");
			currentTescShader = string("None");
			currentGeomShader = string("None");
			currentFragShader = string("None");
		}
		if (isNewFile) ImGui::SetItemDefaultFocus();

		for (int i = 0; i < shaderPrograms.size(); i++)
		{
			bool isCurrent = shaderPrograms[i].string() == currentFile;
			if (ImGui::Selectable(shaderPrograms[i].filename().string().c_str(), isCurrent))
			{
				dirty = false;

				currentFile = shaderPrograms[i].string();

				ifstream input(currentFile.c_str());

				if (!input.good())
				{
					std::cout << std::endl << currentFile << " is not a valid filepath" << std::endl;
					currentFile = string("New File");
					programName = string();
				}
				else
				{
					programName = shaderPrograms[i].filename().string();
					int extensionSize = shaderPrograms[i].extension().string().size();
					for (int i = 0; i < extensionSize; i++)
					{
						programName.pop_back();
					}

					json shaderProgram;
					input >> shaderProgram;

					if (shaderProgram.contains("Vertex")) currentVertShader = shaderProgram["Vertex"];
					else std::cout << std::endl << currentFile << " does not contain a valid Vertex shader" << std::endl;

					if (shaderProgram.contains("Tessellation Evaluation")) currentTeseShader = shaderProgram["Tessellation Evaluation"];
					else std::cout << std::endl << currentFile << " does not contain a valid Tessellation Evaluation shader" << std::endl;

					if (shaderProgram.contains("Tessellation Control")) currentTescShader = shaderProgram["Tessellation Control"];
					else std::cout << std::endl << currentFile << " does not contain a valid Tessellation Control shader" << std::endl;

					if (shaderProgram.contains("Geometry")) currentGeomShader = shaderProgram["Geometry"];
					else std::cout << std::endl << currentFile << " does not contain a valid Geometry shader" << std::endl;

					if (shaderProgram.contains("Fragment")) currentFragShader = shaderProgram["Fragment"];
					else std::cout << std::endl << currentFile << " does not contain a valid Fragment shader" << std::endl;
				}
			}
			if (isCurrent) ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}

	if (ImGui::InputText("Program Name", &programName))
	{
		dirty = true;
	}

	ImGui::Spacing();

	if (ImGui::Button("Save"))
	{
		dirty = false;

		json shaderProgram;

		shaderProgram["Vertex"] = currentVertShader;
		shaderProgram["Tessellation Evaluation"] = currentTeseShader;
		shaderProgram["Tessellation Control"] = currentTescShader;
		shaderProgram["Geometry"] = currentGeomShader;
		shaderProgram["Fragment"] = currentFragShader;

		ostringstream stream;
		stream << "Shaders\\" + programName + ".shaderprogram";

		string oldFilename = currentFile;
		currentFile = stream.str();

		ofstream output(currentFile.c_str());
		output << std::setw(4) << shaderProgram << std::endl;

		if (currentFile == oldFilename)
		{
			shaderManager->ReloadProgram(currentFile.c_str());
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("New"))
	{
		dirty = false;

		currentFile = string("New File");
		programName = string();
		currentVertShader = string("None");
		currentTeseShader = string("None");
		currentTescShader = string("None");
		currentGeomShader = string("None");
		currentFragShader = string("None");
	}
	if (dirty)
	{
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 0, 255));
		ImGui::Text("Unsaved");
		ImGui::PopStyleColor();
	}


	ImGui::Spacing();

	DrawShaderPopup(VertexStage, vertexShaders, currentVertShader);
	DrawShaderPopup(TessellationEvaluationStage, tessellationEvaluationShaders, currentTeseShader);
	DrawShaderPopup(TessellationControlStage, tessellationEvaluationShaders, currentTescShader);
	DrawShaderPopup(GeometryStage, geometryShaders, currentGeomShader);
	DrawShaderPopup(FragmentStage, fragmentShaders, currentFragShader);

}

void ShaderProgramGUI::DrawShaderPopup(ShaderStage stage, vector<path> shaderPaths, string& currentShader)
{
	string stageName;
	switch (stage)
	{
	case VertexStage: stageName = string("Vertex"); break;
	case TessellationEvaluationStage: stageName = string("Tess Evaluation"); break;
	case TessellationControlStage: stageName = string("Tess Control"); break;
	case GeometryStage: stageName = string("Geometry"); break;
	case FragmentStage: stageName = string("Fragment"); break;
	}

	if (ImGui::BeginCombo(stageName.c_str(), currentShader.c_str()))
	{
		bool isNone = currentShader == string("None");
		if (ImGui::Selectable("None", isNone))
		{
			dirty = true;
			currentShader = string("None");
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
}