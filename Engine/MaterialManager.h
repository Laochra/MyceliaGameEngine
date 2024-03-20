#pragma once

#include "Material.h"

#include <map>
using std::map;

#include <string>
using std::string;

#include "GeneralMacros.h"

class MaterialManager
{
public:
	MaterialManager() = default;
	~MaterialManager();
	uncopyable(MaterialManager);
	immovable(MaterialManager);

	Material* GetMaterial(const char* filepath);
	void ReloadMaterial(const char* filepath);

private:
	map<string, Material*> loadedMaterials = map<string, Material*>();

	Material* AddMaterial(const char* filepath);
};

extern MaterialManager* materialManager;