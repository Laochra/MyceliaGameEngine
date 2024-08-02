#pragma once

#include "Shader.h"

#include <map>
using std::map;

#include <string>
using std::string;

#include "GeneralMacros.h"

class ShaderManager
{
public:
	vector<string> defaultPrograms;

	ShaderManager();
	~ShaderManager();
	uncopyable(ShaderManager);
	immovable(ShaderManager);
	
	ShaderProgram* GetProgram(const char* filepath);
	void ReloadProgram(const char* filepath);

private:
	map<string, ShaderProgram*> loadedPrograms;

	ShaderProgram* AddProgram(const char* filepath);
};

extern ShaderManager* shaderManager;