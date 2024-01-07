#include "ShaderManager.h"

ShaderManager* shaderManager = nullptr;

ShaderProgram* ShaderManager::GetProgram(const char* filepath)
{
    if (loadedPrograms.count(filepath) == 0)
    {
        return AddProgram(filepath);
    }
    else
    {
        return loadedPrograms[filepath];
    }
}

void ShaderManager::ReloadProgram(const char* filepath)
{
    if (loadedPrograms.count(string(filepath)) != 0)
    {
        loadedPrograms[string(filepath)]->LoadAndLinkFromJSON(filepath);
    }
}

ShaderProgram* ShaderManager::AddProgram(const char* filepath)
{
    ShaderProgram* newProgram = new ShaderProgram;
    if (newProgram->LoadAndLinkFromJSON(filepath))
    {
        loadedPrograms.insert(std::pair(string(filepath), newProgram));
        return newProgram;
    }
    else return nullptr;
}
