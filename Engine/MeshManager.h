#pragma once

#include "Mesh.h"

#include <map>
using std::map;

#include <string>
using std::string;

#include "GeneralMacros.h"

class MeshManager
{
public:
	MeshManager();
	~MeshManager();
	uncopyable(MeshManager);
	immovable(MeshManager);

	Mesh* GetMesh(const char* filepath);
	void ReloadMesh(const char* filepath);

private:
	map<string, Mesh*> loadedMeshes;

	Mesh* AddMesh(const char* filepath);
};

extern MeshManager* meshManager;