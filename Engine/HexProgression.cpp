#include "HexProgression.h"

#include "TileData.h"
#include "HexScrapbook.h"
#include "HexFog.h"
#include "HexAudio.h"

#include "Easing.h"
#include "TimeManager.h"
#include "UIManager.h"

#include "AppInfo.h"
#include "Application.h"
#include "HexGame.h"

std::vector<HexProgression::Milestone> HexProgression::lifeMilestones;

std::vector<TileID> HexProgression::startingVariants;

uint HexProgression::startingRadius = 3U;
uint HexProgression::currentRadius = startingRadius;

uint HexProgression::tileLifeBonus = 1U;
uint HexProgression::habitatLifeBonus = 3U;

UISprite* HexProgression::newVariantsIcon = nullptr;

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
	for (vector<TileData>& tileType : TileData::tilesData)
	{
		for (TileData& tileVariant : tileType)
		{
			tileVariant.unlocked = false;
		}
	}
	for (TileID tileID : HexProgression::startingVariants)
	{
		TileData::Get(tileID).unlocked = true;
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

		vector<json> variantUnlocksJSON;
		for (const TileID& tileID : lifeMilestone.variantUnlocks)
		{
			json idJSON;
			idJSON["Type"] = (int)tileID.type;
			idJSON["Variant"] = (int)tileID.variant;
			variantUnlocksJSON.push_back(idJSON);
		}
		lifeMilestoneJSON["VariantUnlocks"] = variantUnlocksJSON;
		lifeMilestoneJSON["LifeRequirement"] = lifeMilestone.lifeRequirement;
		lifeMilestoneJSON["RadiusIncrease"] = lifeMilestone.radiusIncrease;

		lifeMilestonesJSON.push_back(lifeMilestoneJSON);
	}
	jsonObj["LifeMilestones"] = lifeMilestonesJSON;

	vector<json> startingVariantsJSON;
	for (const TileID& tileID : startingVariants)
	{
		json idJSON;
		idJSON["Type"] = (int)tileID.type;
		idJSON["Variant"] = (int)tileID.variant;
		startingVariantsJSON.push_back(idJSON);
	}
	jsonObj["StartingVariants"] = startingVariantsJSON;
	jsonObj["StartingRadius"] = startingRadius;

	jsonObj["TileLifeBonus"] = tileLifeBonus;
	jsonObj["HabitatLifeBonus"] = habitatLifeBonus;

	jsonObj["NewVariantsIcon"] = HexProgression::newVariantsIcon != nullptr ? HexProgression::newVariantsIcon->name : "None";
}

void HexProgression::LoadFrom(const json& jsonObj)
{
	if (jsonObj.contains("LifeMilestones"))
	{
		std::vector<json> lifeMilestonesJSON = jsonObj["LifeMilestones"];
		for (json lifeMilestoneJSON : lifeMilestonesJSON)
		{
			HexProgression::Milestone lifeMilestone;

			if (lifeMilestoneJSON.contains("VariantUnlocks"))
			{
				vector<json> variantUnlocksJSON = lifeMilestoneJSON["VariantUnlocks"];
				for (const json& tileIDJSON : variantUnlocksJSON)
				{
					lifeMilestone.variantUnlocks.push_back(
						TileID(
							(HexType)(int)tileIDJSON["Type"],
							(char)(int)tileIDJSON["Variant"]
						)
					);
				}
			}
			lifeMilestone.lifeRequirement = lifeMilestoneJSON["LifeRequirement"];
			lifeMilestone.radiusIncrease = lifeMilestoneJSON["RadiusIncrease"];

			lifeMilestones.push_back(lifeMilestone);
		}
	}

	if (jsonObj.contains("StartingVariants"))
	{
		vector<json> startingVariantsJSON = jsonObj["StartingVariants"];
		for (const json& tileIDJSON : startingVariantsJSON)
		{
			startingVariants.push_back(
				TileID(
					(HexType)(int)tileIDJSON["Type"],
					(char)(int)tileIDJSON["Variant"]
				)
			);
		}
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

	if (jsonObj.contains("NewVariantsIcon"))
	{
		string newVariantsIconName = jsonObj["NewVariantsIcon"];
		for (UISprite*& uiSprite : UIManager::sprites)
		{
			if (uiSprite->name == newVariantsIconName)
			{
				HexProgression::newVariantsIcon = uiSprite;
				break;
			}
		}
	}
}

void HexProgression::CompleteMilestone(const Milestone& milestone)
{
	if (newVariantsIcon != nullptr) newVariantsIcon->enabled = true;

	for (TileID tileID : milestone.variantUnlocks)
	{
		TileData::Get(tileID).unlocked = true;
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
