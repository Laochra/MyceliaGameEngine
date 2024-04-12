#include "ShaderManager.h"

#include "Debug.h"

using std::pair;

ShaderManager* shaderManager = nullptr;

ShaderManager::ShaderManager()
{
   ShaderProgram* defaultShaderProgram = new ShaderProgram;
   if (defaultShaderProgram->LoadAndLinkFromJSON("Engine\\DefaultAssets\\Default.gpu"))
   {
      loadedPrograms.insert(std::pair(string("Default"), defaultShaderProgram));
   }
   else
   {
      debug->Log({ "Failed to load Default.gpu" }, Debug::Warning, Debug::WRN100);
      delete defaultShaderProgram;
   }
}

ShaderManager::~ShaderManager()
{
   for (pair<string, ShaderProgram*> programPair : loadedPrograms) { del(programPair.second); }
}

ShaderProgram* ShaderManager::GetProgram(const char* filepath)
{
   if (loadedPrograms.count(filepath) == 0) { return AddProgram(filepath); }
   else { return loadedPrograms[filepath]; }
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
   else
   {
      delete newProgram;
      debug->Log({ "Failed to load a ShaderProgram from filepath: ", filepath, ". Set to Default.gpu" }, Debug::Warning, Debug::WRN100);
      return GetProgram("Default");
   }
}
