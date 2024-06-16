#include "MaterialManager.h"

#include "Debug.h"

using std::pair;

MaterialManager* materialManager = nullptr;

MaterialManager::MaterialManager()
{
   Material* missingMaterial = new Material;
   if (missingMaterial->LoadFromJSON("Engine\\DefaultAssets\\Missing.mat"))
   {
      loadedMaterials.insert(std::pair(string("Missing"), missingMaterial));
   }
   else
   {
      Debug::LogError(ERR101, "Failed to load Missing.mat", locationinfo);
      delete missingMaterial;
   }

   Material* defaultMaterial = new Material;
   if (defaultMaterial->LoadFromJSON("Engine\\DefaultAssets\\Default.mat"))
   {
      loadedMaterials.insert(std::pair(string("Default"), defaultMaterial));
   }
   else
   {
      Debug::LogError(ERR101, "Failed to load Default.mat", locationinfo);
      delete defaultMaterial;
   }
}

MaterialManager::~MaterialManager()
{
   for (pair<string, Material*> materialPair : loadedMaterials) { del(materialPair.second); }
}

Material* MaterialManager::GetMaterial(const char* filepath)
{
   if (loadedMaterials.count(filepath) == 0) return AddMaterial(filepath);
   else return loadedMaterials[filepath];
}

void MaterialManager::ReloadMaterial(const char* filepath)
{
   if (loadedMaterials.count(filepath) != 0)
   {
      loadedMaterials[filepath]->LoadFromJSON(filepath);
   }
}

Material* MaterialManager::AddMaterial(const char* filepath)
{
   Material* newMaterial = new Material;
   if (newMaterial->LoadFromJSON(filepath))
   {
      loadedMaterials.insert(std::pair(string(filepath), newMaterial));
      return newMaterial;
   }
   else
   {
      delete newMaterial;

      Debug::LogWarning(WRN100, "Failed to load a Material at: ", filepath, ". Set to Missing.mat", locationinfo);
      return GetMaterial("Missing");
   }
}
