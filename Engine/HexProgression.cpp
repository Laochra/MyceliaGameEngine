#include "HexProgression.h"

#include "TileData.h"

std::vector<HexProgression::Milestone> HexProgression::lifeMilestones;

std::vector<std::string> HexProgression::startingVariants;

uint HexProgression::startingRadius = 3U;
uint HexProgression::currentRadius = startingRadius;

uint HexProgression::tileLifeBonus = 1U;
uint HexProgression::habitatLifeBonus = 3U;

uint HexProgression::currentMilestone;
uint HexProgression::currentLife = 0U;

void HexProgression::Initialise() noexcept
{
	ResetProgression();
}

uint HexProgression::GetLife() noexcept
{
	return currentLife;
}

const HexProgression::Milestone* HexProgression::IncreaseLife(uint lifeToAdd) noexcept
{
	currentLife += lifeToAdd;

	if (currentMilestone < lifeMilestones.size())
	{
		const Milestone* milestoneToCheck = &lifeMilestones[currentMilestone];
		if (currentLife >= milestoneToCheck->lifeRequirement)
		{
			currentMilestone++;
			CompleteMilestone(*milestoneToCheck);
			return milestoneToCheck;
		}
	}

	return nullptr;
}

void HexProgression::ResetProgression() noexcept
{
	std::vector<std::string> variantsToUnlock = HexProgression::startingVariants;

	for (vector<TileData>& tileType : TileData::tilesData)
	{
		for (TileData& tileVariant : tileType)
		{
			auto variantIt = std::find(variantsToUnlock.begin(), variantsToUnlock.end(), tileVariant.name);
			if (variantIt != variantsToUnlock.end())
			{
				tileVariant.unlocked = true;
				variantsToUnlock.erase(variantIt);
				if (variantsToUnlock.size() == 0) break;
			}
			else
			{
				tileVariant.unlocked = false;
			}
		}
		if (variantsToUnlock.size() == 0) break;
	}
	
	currentMilestone = 0U;
	currentLife = 0U;
	currentRadius = startingRadius;
}

void HexProgression::SaveTo(json& jsonObj)
{
	std::vector<json> lifeMilestonesJSON;
	for (HexProgression::Milestone lifeMilestone : lifeMilestones)
	{
		json lifeMilestoneJSON;

		lifeMilestoneJSON["Type"] = lifeMilestone.type;
		lifeMilestoneJSON["Names"] = lifeMilestone.names;
		lifeMilestoneJSON["LifeRequirement"] = lifeMilestone.lifeRequirement;
		lifeMilestoneJSON["RadiusIncrease"] = lifeMilestone.radiusIncrease;

		lifeMilestonesJSON.push_back(lifeMilestoneJSON);
	}
	jsonObj["LifeMilestones"] = lifeMilestonesJSON;

	jsonObj["StartingVariants"] = startingVariants;
	jsonObj["StartingRadius"] = startingRadius;

	jsonObj["TileLifeBonus"] = tileLifeBonus;
	jsonObj["HabitatLifeBonus"] = habitatLifeBonus;
}

void HexProgression::LoadFrom(const json& jsonObj)
{
	if (jsonObj.contains("LifeMilestones"))
	{
		std::vector<json> lifeMilestonesJSON = jsonObj["LifeMilestones"];
		for (json lifeMilestoneJSON : lifeMilestonesJSON)
		{
			HexProgression::Milestone lifeMilestone;

			lifeMilestone.type = lifeMilestoneJSON["Type"];
			lifeMilestone.names = lifeMilestoneJSON["Names"];
			lifeMilestone.lifeRequirement = lifeMilestoneJSON["LifeRequirement"];
			lifeMilestone.radiusIncrease = lifeMilestoneJSON["RadiusIncrease"];

			lifeMilestones.push_back(lifeMilestone);
		}
	}

	if (jsonObj.contains("StartingVariants"))
	{
		startingVariants = jsonObj["StartingVariants"];
	}
	if (jsonObj.contains("StartingRadius"))
	{
		startingRadius = jsonObj["StartingRadius"];
		currentRadius = startingRadius;
	}

	if (jsonObj.contains("TileLifeBonus"))
	{
		tileLifeBonus = jsonObj["TileLifeBonus"];
	}
	if (jsonObj.contains("HabitatLifeBonus"))
	{
		habitatLifeBonus = jsonObj["HabitatLifeBonus"];
	}
}

void HexProgression::CompleteMilestone(const Milestone& milestone)
{
	currentRadius += milestone.radiusIncrease;

	if (milestone.type == Milestone::VariantUnlock)
	{
		std::vector<std::string> variantsToUnlock = milestone.names;

		for (vector<TileData>& tileType : TileData::tilesData)
		{
			for (TileData& tileVariant : tileType)
			{
				auto variantIt = std::find(variantsToUnlock.begin(), variantsToUnlock.end(), tileVariant.name);
				if (variantIt != variantsToUnlock.end())
				{
					tileVariant.unlocked = true;
					variantsToUnlock.erase(variantIt);
					if (variantsToUnlock.size() == 0) break;
				}
			}
			if (variantsToUnlock.size() == 0) break;
		}
	}
}
