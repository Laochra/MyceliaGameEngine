#pragma once

#include "HabitatObject.h"
#include "HexCoords.h"

struct Habitat
{
	HabitatObject* object = nullptr;

	HexOffsetCoord tileCoords[3] = { {0,0}, {0,0}, {0,0} };
};
