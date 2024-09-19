#pragma once

#include "HexTileObject.h"
#include "HexType.h"

struct HexTile
{
	HexTileObject* object = nullptr;

	char habitat = -1; // -1 means not part of a habitat
	HexType type = HexType::Unreached;
	char variant = 0;
};
