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
      Debug::LogError(LogID::ERR101, "PBRLit.gpu ", locationinfo);
      delete pbrLit;
   }
   ShaderProgram* unlit = new ShaderProgram;
   if (unlit->LoadAndLinkFromJSON("Engine\\DefaultAssets\\Unlit.gpu"))
   {
      loadedPrograms.insert(std::pair(string("Unlit"), unlit));
   }
   else
   {
      Debug::LogError(LogID::ERR101, "Unlit.gpu ", locationinfo);
      delete unlit;
   }
   ShaderProgram* depthOnly = new ShaderProgram;
   if (depthOnly->LoadAndLinkFromJSON("Engine\\DefaultAssets\\DepthOnly.gpu"))
   {
      loadedPrograms.insert(std::pair(string("DepthOnly"), depthOnly));
   }
   else
   {
      Debug::LogError(LogID::ERR101, "DepthOnly.gpu ", locationinfo);
      delete depthOnly;
   }
   ShaderProgram* drawGUID = new ShaderProgram;
   if (drawGUID->LoadAndLinkFromJSON("Engine\\DefaultAssets\\DrawGUID.gpu"))
   {
      loadedPrograms.insert(std::pair(string("DrawGUID"), drawGUID));
   }
   else
   {
      Debug::LogError(LogID::ERR101, "DrawGUID.gpu ", locationinfo);
      delete drawGUID;
   }

   //ShaderProgram* drawLineID = new ShaderProgram;
   //if (drawLineID->LoadAndLinkFromJSON("Engine\\DefaultAssets\\DrawLineID.gpu"))
   //{
   //   loadedPrograms.insert(std::pair(string("DrawLineID"), drawLineID));
   //}
   //else
   //{
   //   Debug::LogError(LogID::ERR101, "DrawLineID.gpu ", locationinfo);
   //   delete drawLineID;
   //}
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
      Debug::LogWarning(LogID::WRN100, "Failed to load a ShaderProgram from filepath: ", filepath, ". Set to PBRLit.gpu", locationinfo);
      return GetProgram("PBRLit");
   }
}
