#include "MeshManager.h"

#include <iostream>

using std::pair;

MeshManager* meshManager = nullptr;

MeshManager::MeshManager()
{
   
}

MeshManager::~MeshManager()
{
   for (pair<string, Mesh*> meshPair : loadedMeshes) { del(meshPair.second); }
}

Mesh* MeshManager::GetMesh(const char* filepath)
{
   if (loadedMeshes.count(filepath) == 0) { return AddMesh(filepath); }
   else { return loadedMeshes[filepath]; }
}

void MeshManager::ReloadMesh(const char* filepath)
{
   if (loadedMeshes.count(string(filepath)) != 0)
   {
      loadedMeshes[string(filepath)]->LoadFromFile(filepath);
   }
}

Mesh* MeshManager::AddMesh(const char* filepath)
{
   Mesh* newMesh = new Mesh;
   if (newMesh->LoadFromFile(filepath))
   {
      loadedMeshes.insert(std::pair(string(filepath), newMesh));
      return newMesh;
   }
   else
   {
      delete newMesh;
      //if (memcmp(filepath, "None", 5) != 0)
      //{
      //   std::cout << "Couldn't find a Mesh at filepath: " << filepath << ".\n";
      //}
      return nullptr;
   }
}
