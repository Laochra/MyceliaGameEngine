#pragma once

#include "UISprite.h"

#include <vector>
#include <string>

class HexScrapbook
{
public:
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

	static void SetEnabled(bool enabledStatus) noexcept;
	static bool IsEnabled() noexcept;

	static void CacheEnabledStatus() noexcept;
	static void RestoreEnabledStatus() noexcept;

	static void ConcealSprites() noexcept;
	static void RevealSprites() noexcept;

	static void RevealHabitat(int index) noexcept;
	static void RevealHabitatTile(int habitatIndex, int tileIndex) noexcept;

private:
	static bool enabled;
};
