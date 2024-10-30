#include "MeshManager.h"

#include "Debug.h"

using std::pair;

MeshManager* meshManager = nullptr;

MeshManager::MeshManager()
{
   Mesh* proceduralQuad = new Mesh();
   proceduralQuad->InitialiseQuad();
   loadedMeshes.insert(std::pair(string("ProceduralQuad"), proceduralQuad));

   Mesh* proceduralCube = new Mesh();
   proceduralCube->InitialiseCube();
   loadedMeshes.insert(std::pair(string("ProceduralCube"), proceduralCube));

   Mesh* proceduralCone = new Mesh();
   proceduralCone->InitialiseCone();
   loadedMeshes.insert(std::pair(string("ProceduralCone"), proceduralCone));

   defaultMeshes.push_back("ProceduralQuad");
   defaultMeshes.push_back("ProceduralCube");
   defaultMeshes.push_back("ProceduralCone");
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
void MeshManager::ReloadAll() noexcept
{
   for (auto it = loadedMeshes.begin(); it != loadedMeshes.end();)
   {
      if (std::find(defaultMeshes.begin(), defaultMeshes.end(), it->first) != defaultMeshes.end())
      {
         ++it;
         continue;
      }
      
      if (!it->second->LoadFromFile(it->first.c_str()))
      {
         it = loadedMeshes.erase(it);
      }
      else
      {
         ++it;
      }
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
      Debug::LogWarning(LogID::WRN100, filepath, ". Set to None", locationinfo);
      return nullptr;
   }
}
