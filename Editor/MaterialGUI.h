#pragma once

#include "Material.h"

#include "GUI.h"

namespace MaterialGUI
{	
	struct Fields
	{
		string materialName = "";
		string filePath = "New File";
		string shaderFilePath = "PBRLit";
	
		vector<MaterialInput> attributes = vector<MaterialInput>();
		vector<MaterialInput> uniforms = vector<MaterialInput>();
	};
	
	extern Fields current;
	extern bool dirty;
	
	extern void Initialise();
	
	extern void Save();
	extern void NewLoad(path filePath);
	extern void Reload();
	extern void Load(string filePathStr);
	extern void LoadShaderProgram(string filePathStr);
	
	extern void Draw(const char* const name, bool& open);
	extern void DrawField(MaterialInput& field);
}
