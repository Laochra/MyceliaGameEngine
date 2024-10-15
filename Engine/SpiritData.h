#pragma once

#include "JsonIncludes.h"

#include <string>
using std::string;
#include <vector>
using std::vector;

struct SpiritData
{
	string name = "New Spirit";
	string prefabFilepath = "None";
	bool prefabLoaded = false;

	static SpiritData& Get(int index) noexcept;

	static const json* GetPrefab(int index) noexcept;
	static const json* GetPrefab(string name) noexcept;
	static int GetPrefabIndex(string name) noexcept;

	static void ClearAndReset() noexcept;

	static vector<SpiritData> spiritsData;

private:
	static void PopulatePrefab(int index) noexcept;

	json prefab;
};
