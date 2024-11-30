#pragma once

#include "UISprite.h"

#include <vector>
#include <string>

#include "MycCoroutine.h"

enum class HabitatCollectionState : int { Locked, Unlocked, Recieved };
extern int operator+(int a, HabitatCollectionState b) noexcept;
extern int operator+(HabitatCollectionState a, int b) noexcept;

class HexScrapbook
{
public:
	enum class State { Closed, Opening, Open, Closing };
	struct HabitatCollection
	{
		UISprite* habitat;
		std::string habitatTextures[3] { "None", "None", "None" };
		UISprite* tiles[3];
		std::string tileTextures[9] { "None", "None", "None", "None", "None", "None", "None", "None", "None" };
	};

	static float animationSpeed;

	static UISprite* base;
	static std::string baseTexture;
	static std::vector<HabitatCollection> habitats;

	static const Coroutine::Pair* Open() noexcept;
	static const Coroutine::Pair* Close() noexcept;

	static void Reset() noexcept;

	static State GetState() noexcept;

	static void CacheEnabledStatus() noexcept;
	static void RestoreEnabledStatus() noexcept;

	static void ConcealSprites() noexcept;
	static void RevealSprites() noexcept;

	static void SetHabitatState(int index, HabitatCollectionState state) noexcept;
	static void SetHabitatTileState(int habitatIndex, int tileIndex, HabitatCollectionState state) noexcept;
	static void SetHabitatState(HabitatCollection& habitat, HabitatCollectionState state) noexcept;
	static void SetHabitatTileState(HabitatCollection& habitat, int tileIndex, HabitatCollectionState state) noexcept;

	friend struct ScrapbookAnimationData;
	friend class ScrapbookAnimation;

private:
	static float openness; // 0 for not open, 1 for open
	static State state;
	static void SetState(State newState) noexcept;
};
