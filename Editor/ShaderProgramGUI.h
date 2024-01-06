#pragma once

#include "ImGuiIncludes.h"
#include "GUI.h"
#include "Shader.h"

#include <vector>
using std::vector;

#include <filesystem>
namespace filesystem = std::filesystem;
using std::string;
using filesystem::directory_entry;
using filesystem::directory_iterator;
using filesystem::path;

namespace ShaderProgramGUI
{
	inline vector<path> vertexShaders;
	inline vector<path> fragmentShaders;

	inline void Initialise();
	inline void Draw();
}

void ShaderProgramGUI::Initialise()
{
	vertexShaders.clear();
	fragmentShaders.clear();

	string shadersPath = "Shaders";

	for (const directory_entry& entry : directory_iterator(shadersPath))
	{
		path filePath = entry.path();
		path extension = filePath.extension();

		if (extension.string() == string(".vert"))
		{
			vertexShaders.push_back(filePath);
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

	string none = string("None");

	static string currentVertShader = none;
	if (ImGui::BeginCombo("Vertex Shader", currentVertShader.c_str()))
	{
		bool isNone = currentVertShader == none;
		if (ImGui::Selectable(none.c_str(), isNone))
		{
			currentVertShader = none;
			// set vertex shader to null
		}
		if (isNone) ImGui::SetItemDefaultFocus();

		for (int i = 0; i < vertexShaders.size(); i++)
		{
			bool isCurrent = vertexShaders[i].string() == currentVertShader;
			if (ImGui::Selectable(vertexShaders[i].filename().string().c_str(), isCurrent))
			{
				currentVertShader = vertexShaders[i].string();
				// set vertex shader to selected
			}

			if (isCurrent) ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}

	static string currentFragShader = none;
	if (ImGui::BeginCombo("Fragment Shader", currentFragShader.c_str()))
	{
		bool isNone = currentFragShader == none;
		if (ImGui::Selectable(none.c_str(), isNone))
		{
			currentFragShader = none;
			// set fragment shader to null
		}
		if (isNone) ImGui::SetItemDefaultFocus();

		for (int i = 0; i < fragmentShaders.size(); i++)
		{
			bool isCurrent = fragmentShaders[i].string() == currentFragShader;
			if (ImGui::Selectable(fragmentShaders[i].filename().string().c_str(), isCurrent))
			{
				currentFragShader = fragmentShaders[i].string();
				// set fragment shader to selected
			}

			if (isCurrent) ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}
}