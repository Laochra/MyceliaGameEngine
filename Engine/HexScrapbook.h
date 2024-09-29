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
};
