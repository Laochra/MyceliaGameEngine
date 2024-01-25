#pragma once

#include "Shader.h"

#include <map>
using std::map;

#include <string>
using std::string;

class ShaderManager
{
public:
	ShaderProgram* GetProgram(const char* filepath);
	void ReloadProgram(const char* filepath);

private:
	map<string, ShaderProgram*> loadedPrograms;

	ShaderProgram* AddProgram(const char* filepath);
};

extern ShaderManager* shaderManager;