#pragma once

#include <vector>
#include <string>

#include "UISprite.h"

#include "MycCoroutine.h"

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

	static uint tileLifeBonus;
	static uint habitatLifeBonus;

	static UISprite* newVariantsIcon;

	struct StickerEventData
	{
		float moveAmount = 0.35f;
		float moveSpeed = 1.5f;

		float startDelay = 0.5f;
		float stickerPlaceDelay = 0.5f;
		float closeDelay = 0.75f;
		float expandBorderDelay = 0.0f;
		float endDelay = 0.0f;
	};
	static StickerEventData stickerEvent;

	static void Initialise() noexcept;

	static uint GetLife() noexcept;
	// If a milestone is hit a pointer to it will be returned, otherwise returns nullptr
	static const Milestone* IncreaseLife(uint lifeToAdd) noexcept;
	static void ResetProgression() noexcept;

	static const Coroutine::Pair* PlayHabitatStickerAnimation(char habitatIndex) noexcept;

	static void SaveTo(json& jsonObj);
	static void LoadFrom(const json& jsonObj);

private:
	static uint currentMilestone;
	static uint currentLife;

	static void CompleteMilestone(const Milestone& milestone);
};
