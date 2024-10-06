#include "HexScrapbook.h"

UISprite* HexScrapbook::base;
std::string HexScrapbook::baseTexture;

std::vector<HexScrapbook::HabitatCollection> HexScrapbook::habitats;

bool HexScrapbook::enabled = false;

void HexScrapbook::SetEnabled(bool enabledStatus) noexcept
{
	enabled = enabledStatus;

	base->enabled = enabledStatus;

	for (HabitatCollection& habitat : habitats)
	{
		habitat.habitat->enabled = enabledStatus;
		habitat.tiles[0]->enabled = enabledStatus;
		habitat.tiles[1]->enabled = enabledStatus;
		habitat.tiles[2]->enabled = enabledStatus;
	}
}

bool HexScrapbook::IsEnabled() noexcept
{
	return enabled;
}

void HexScrapbook::ConcealSprites() noexcept
{
	for (HabitatCollection& habitat : habitats)
	{
		habitat.habitat->Load(habitat.habitatTextures[0].c_str());
		habitat.tiles[0]->Load(habitat.tileTextures[0].c_str());
		habitat.tiles[1]->Load(habitat.tileTextures[2].c_str());
		habitat.tiles[2]->Load(habitat.tileTextures[4].c_str());
	}
}

void HexScrapbook::RevealSprites() noexcept
{
	for (HabitatCollection& habitat : habitats)
	{
		habitat.habitat->Load(habitat.habitatTextures[1].c_str());
		habitat.tiles[0]->Load(habitat.tileTextures[1].c_str());
		habitat.tiles[1]->Load(habitat.tileTextures[3].c_str());
		habitat.tiles[2]->Load(habitat.tileTextures[5].c_str());
	}
}

void HexScrapbook::RevealHabitat(int index) noexcept
{
	HabitatCollection& habitat = habitats[index];

	habitat.habitat->Load(habitat.habitatTextures[1].c_str());
	habitat.tiles[0]->Load(habitat.tileTextures[1].c_str());
	habitat.tiles[1]->Load(habitat.tileTextures[3].c_str());
	habitat.tiles[2]->Load(habitat.tileTextures[5].c_str());
}
void HexScrapbook::RevealHabitatTile(int habitatIndex, int tileIndex) noexcept
{
	HabitatCollection& habitat = habitats[habitatIndex];

	habitat.tiles[tileIndex]->Load(habitat.tileTextures[tileIndex * 2 + 1].c_str());
}
