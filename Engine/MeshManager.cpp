#include "MeshManager.h"

#include "Debug.h"

using std::pair;

MeshManager* meshManager = nullptr;

MeshManager::MeshManager()
{
   Mesh* quad = new Mesh();
   quad->InitialiseQuad();
   loadedMeshes.insert(std::pair(string("Quad"), quad));

   Mesh* hexagon = new Mesh();
   hexagon->InitialiseHexagon();
   loadedMeshes.insert(std::pair(string("Hexagon"), hexagon));

   Mesh* cube = new Mesh();
   cube->InitialiseCube();
   loadedMeshes.insert(std::pair(string("Cube"), cube));

   Mesh* cone = new Mesh();
   cone->InitialiseCone();
   loadedMeshes.insert(std::pair(string("Cone"), cone));

   defaultMeshes.push_back("Quad");
   defaultMeshes.push_back("Hexagon");
   defaultMeshes.push_back("Cube");
   defaultMeshes.push_back("Cone");
}

MeshManager::~MeshManager()
{
   for (pair<string, Mesh*> meshPair : loadedMeshes) { del(meshPair.second); }
}

Mesh* MeshManager::GetMesh(const char* filepath)
{
   if (filepath == nullptr) return nullptr;
   if (strcmp(filepath, "None") == 0) { return nullptr; }

   if (loadedMeshes.count(filepath) == 0) { return AddMesh(filepath); }
   else { return loadedMeshes[filepath]; }
}

void MeshManager::ReloadMesh(const char* filepath)
{
   string strFilepath = filepath;
   if (loadedMeshes.count(strFilepath) != 0 && strFilepath != "None")
   {
      // If not a default mesh, reload it
      if (std::find(defaultMeshes.begin(), defaultMeshes.end(), strFilepath) == defaultMeshes.end())
      {
         loadedMeshes[strFilepath]->LoadFromFile(filepath);
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
