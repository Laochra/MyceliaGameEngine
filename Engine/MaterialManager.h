#pragma once

#include "Material.h"

#include <map>
using std::map;

#include <string>
using std::string;

class MaterialManager
{
	public:
	Material* GetMaterial(const char* filepath);
	void ReloadMaterial(const char* filepath);

	private:
	map<string, Material*> loadedMaterials;

	Material* AddMaterial(const char* filepath);
};

extern MaterialManager* materialManager;