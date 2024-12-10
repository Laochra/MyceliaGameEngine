#pragma once

#include "JsonIncludes.h"

#include <string>
#include <vector>

struct HexCloudData
{
	unsigned int maxClouds = 100;

	float minDistance = 1.5f;
	float cloudHeight = 3.0f;

	float sizeRange[2] = { 1.0f, 2.0f };

	float moveSpeed = 0.1f;
	float moveDir[2] = { 1.0f, 0.25f };
};

class HexClouds
{
public:
	static HexCloudData data;
	static std::vector<std::string> prefabFilepaths;
	static std::vector<json> prefabs;

	static void Initialise() noexcept;
	static void Update() noexcept;
	static void LoadPrefab(int index, std::string filepath) noexcept;
	static void CleanUp() noexcept;
};
