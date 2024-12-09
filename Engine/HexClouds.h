#pragma once

#include "JsonIncludes.h"

#include <string>
#include <vector>

struct HexCloudData
{
	float minDistance = 0.3f;
	float cloudHeight = 3.0f;

	float sizeRange[2] = { 0.7f, 1.3f };

	float moveSpeed = 1.0f;
	float moveDir[2] = { 1.0f, 1.0f };
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
