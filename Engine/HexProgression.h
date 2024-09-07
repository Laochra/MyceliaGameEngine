#pragma once

#include <vector>
#include <string>

#include "JsonIncludes.h"

typedef unsigned int uint;

class HexProgression
{
public:
	struct Milestone
	{
		enum MilestoneType { VariantUnlock, AddedEffects };
		MilestoneType type = VariantUnlock;
		std::vector<std::string> names;

		uint lifeRequirement = 100U;
		uint radiusIncrease = 1U;
	};

	static std::vector<Milestone> lifeMilestones;

	static std::vector<std::string> startingVariants;

	static uint startingRadius;
	static uint currentRadius;

	static void SaveTo(json& jsonObj);
	static void LoadFrom(const json& jsonObj);
};
