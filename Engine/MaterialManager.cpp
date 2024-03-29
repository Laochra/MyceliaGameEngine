#include "MaterialManager.h"

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
      std::cout << "Failed to load Missing.mat\n";
      delete missingMaterial;
   }

   Material* defaultMaterial = new Material;
   if (defaultMaterial->LoadFromJSON("Engine\\DefaultAssets\\Default.mat"))
   {
      loadedMaterials.insert(std::pair(string("Default"), defaultMaterial));
   }
   else
   {
      std::cout << "Failed to load Default.mat\n";
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
      return GetMaterial("Missing");
   }
}
