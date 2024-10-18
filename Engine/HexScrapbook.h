#pragma once

#include "UISprite.h"

#include <vector>
#include <string>

class HexScrapbook
{
public:
	enum class State { Closed, Opening, Open, Closing };
	struct HabitatCollection
	{
		UISprite* habitat;
		std::string habitatTextures[2];
		UISprite* tiles[3];
		std::string tileTextures[6];
	};

	static UISprite* base;
	static std::string baseTexture;
	static std::vector<HabitatCollection> habitats;

	static void Open() noexcept;
	static void Close() noexcept;

	static void Reset() noexcept;

	static State GetState() noexcept;

	static void CacheEnabledStatus() noexcept;
	static void RestoreEnabledStatus() noexcept;

	static void ConcealSprites() noexcept;
	static void RevealSprites() noexcept;

	static void RevealHabitat(int index) noexcept;
	static void RevealHabitatTile(int habitatIndex, int tileIndex) noexcept;

	friend struct ScrapbookAnimationData;
	friend class ScrapbookAnimation;

private:
	static float openness; // 0 for not open, 1 for open
	static float animationSpeed;
	static State state;
	static void SetState(State newState) noexcept;
};
