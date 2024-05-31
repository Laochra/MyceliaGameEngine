#pragma once

#include "Shader.h"
#include "ShaderManager.h"

#include "ImGuiIncludes.h"
#include "GUI.h"
#include "JsonIncludes.h"

#include "Debug.h"

#include <sstream>
using std::ostringstream;

namespace ShaderProgramGUI
{
struct Fields
{
	string programName = "";
	string filePath = "New File";
	string vertShader = "Default3D";
	string teseShader = "None";
	string tescShader = "None";
	string geomShader = "None";
	string fragShader = "PBRLit";

	vector<ShaderInput> attributes = vector<ShaderInput>();
	vector<ShaderInput> uniforms = vector<ShaderInput>();
};

extern vector<path> shaderPrograms;

extern vector<path> vertexShaders;
extern vector<path> tessEvaluationShaders;
extern vector<path> tessellationControlShaders;
extern vector<path> geometryShaders;
extern vector<path> fragmentShaders;

extern Fields current;
extern bool dirty;

extern void Initialise();

extern void Save();
extern void NewLoad(path filePath);
extern void Reload();
extern void Load(string filePathStr);

extern bool validLinkage;
extern void ValidateLinkage();

extern void Draw();
extern void DrawShaderPopup(ShaderStage stage, vector<path> shaderPaths, string& currentShader);
extern void DrawDefaultableShaderPopup(ShaderStage stage, vector<path> shaderPaths, string& currentShader, StringParams defaults = StringParams());
}
