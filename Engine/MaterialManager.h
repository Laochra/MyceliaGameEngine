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
	vector<string> defaultMaterials;

	MaterialManager();
	~MaterialManager();
	uncopyable(MaterialManager);
	immovable(MaterialManager);

	Material* GetMaterial(const char* filepath);
	void ReloadMaterial(const char* filepath);

private:
	map<string, Material*> loadedMaterials;

	Material* AddMaterial(const char* filepath);
};

extern MaterialManager* materialManager;