#include "MaterialManager.h"

using std::pair;

MaterialManager* materialManager = nullptr;

MaterialManager::~MaterialManager()
{
   for (pair<string, Material*> materialPair : loadedMaterials) { del(materialPair.second); }
}

Material* MaterialManager::GetMaterial(const char* filepath)
{
   if (filepath == string("None")) return nullptr;

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
   else return nullptr;
}
