#include "HexProgression.h"

#include "TileData.h"
#include "HexScrapbook.h"
#include "HexFog.h"
#include "HexAudio.h"

#include "Easing.h"
#include "TimeManager.h"

#include "AppInfo.h"
#include "Application.h"
#include "HexGame.h"

std::vector<HexProgression::Milestone> HexProgression::lifeMilestones;

std::vector<std::string> HexProgression::startingVariants;

uint HexProgression::startingRadius = 3U;
uint HexProgression::currentRadius = startingRadius;

uint HexProgression::tileLifeBonus = 1U;
uint HexProgression::habitatLifeBonus = 3U;

HexProgression::StickerEventData HexProgression::stickerEvent;

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
	HexFog::currentRadius = (float)startingRadius;
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
	//currentRadius += milestone.radiusIncrease;

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

struct HabitatStickerEventData
{
	enum class Stage : unsigned char
	{
		WaitingToStart,
		OpenScrapbook,
		PlaceSticker,
		CloseScrapbook,
		ExpandBorder,
		WaitingToEnd
	};

	float stickerPlaceProgress = 0.0f;

	Stage stage = Stage::WaitingToStart;
	char habitatIndex = 0;
};
class HabitatStickerEvent : public Coroutine::Function<HabitatStickerEventData>
{
	using Data = HabitatStickerEventData;
	void operator()(Coroutine::Package& package)
	{
		Data& data = GetData(package);

		switch (data.stage)
		{
		case Data::Stage::WaitingToStart:
		{
			data.stage = Data::Stage::OpenScrapbook;
			CoroutineYieldFor(HexProgression::stickerEvent.startDelay);
			break;
		}
		case Data::Stage::OpenScrapbook:
		{
			HexScrapbook::Open(); // Reuse existing open animation and wait for it to end
			HexScrapbook::habitats[data.habitatIndex].habitat->enabled = false;
			data.stage = Data::Stage::PlaceSticker;
			CoroutineYieldFor(1.0f / HexScrapbook::animationSpeed + HexProgression::stickerEvent.stickerPlaceDelay);
			break;
		}
		case Data::Stage::PlaceSticker:
		{
			UISprite& sticker = *HexScrapbook::habitats[data.habitatIndex].habitat;
			sticker.enabled = true;
			bool finished = data.stickerPlaceProgress >= 1.0f;
			if (finished) data.stickerPlaceProgress = 1.0f;

			sticker.anchor = std::lerp(
				HexProgression::stickerEvent.moveAmount,
				0.0f,
				Easing::BackIn(data.stickerPlaceProgress)
			) * glm::normalize(vec2(1.0f, 1.5f));

			if (finished)
			{
				data.stage = Data::Stage::CloseScrapbook;
				HexAudio::PlayMiscSFX(HexAudio::ScrapbookStickerPlace);
				CoroutineYieldFor(HexProgression::stickerEvent.closeDelay);
			}
			else
			{
				data.stickerPlaceProgress += HexProgression::stickerEvent.moveSpeed * Time::delta;
			}
			break;
		}
		case Data::Stage::CloseScrapbook:
		{
			HexScrapbook::Close(); // Reuse existing close animation and wait for it to end
			data.stage = Data::Stage::ExpandBorder;
			CoroutineYieldFor(1.0f / HexScrapbook::animationSpeed + HexProgression::stickerEvent.expandBorderDelay);
			break;
		}
		case Data::Stage::ExpandBorder:
		{
			HexFog::AnimateFogTo((float)HexProgression::currentRadius);
			data.stage = Data::Stage::WaitingToEnd;
			float increaseAmount = HexProgression::currentRadius - HexFog::currentRadius;
			HexAudio::PlayMiscSFX(HexAudio::FogMove);

			HexGame* game = (HexGame*)AppInfo::application->game;
			game->SetState(HexGame::State::Place);
			if (game->hexGrid->habitats.size() >= 5)
			{
				game->SetState(HexGame::State::ContinueScreen);
			}

			CoroutineYieldFor(increaseAmount * (1.0f / HexFog::animationSpeed) + HexProgression::stickerEvent.endDelay);
			break;
		}
		case Data::Stage::WaitingToEnd:
		{
			CoroutineFinalise();
			break;
		}
		}
	}
};

const Coroutine::Pair* HexProgression::PlayHabitatStickerAnimation(char habitatIndex) noexcept
{
	HabitatStickerEventData* data = new HabitatStickerEventData;
	data->habitatIndex = habitatIndex;
	return Coroutine::Start<HabitatStickerEvent>(data, true);
}
