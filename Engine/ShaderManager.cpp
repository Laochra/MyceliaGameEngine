#include "ShaderManager.h"

#include "Debug.h"

using std::pair;

ShaderManager* shaderManager = nullptr;

ShaderManager::ShaderManager()
{
   ShaderProgram* pbrLit = new ShaderProgram;
   if (pbrLit->LoadAndLinkFromJSON("Engine\\DefaultAssets\\PBRLit.gpu"))
   {
      loadedPrograms.insert(std::pair(string("PBRLit"), pbrLit));
   }
   else
   {
      debug->Log("Failed to load PBRLit.gpu" locationinfo, Debug::Error, Debug::ERR101);
      delete pbrLit;
   }
   ShaderProgram* pbrUnlit = new ShaderProgram;
   if (pbrUnlit->LoadAndLinkFromJSON("Engine\\DefaultAssets\\Unlit.gpu"))
   {
      loadedPrograms.insert(std::pair(string("Unlit"), pbrUnlit));
   }
   else
   {
      debug->Log("Failed to load Unlit.gpu" locationinfo, Debug::Error, Debug::ERR101);
      delete pbrUnlit;
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
      debug->Log({ "Failed to load a ShaderProgram from filepath: ", filepath, ". Set to PBRLit.gpu" locationinfo }, Debug::Warning, Debug::WRN100);
      return GetProgram("PBRLit");
   }
}
