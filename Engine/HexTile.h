#pragma once

#include "HexType.h"
#include "HexCoords.h"

class GameObject3D;

struct HexTile
{
	GameObject3D* object = nullptr;

	char habitat = -1; // -1 means not part of a habitat
	HexType type = HexType::Unreached;
	char variant = 0;
};
