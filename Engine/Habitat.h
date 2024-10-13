#pragma once

#include "HexCoords.h"
#include "HexTile.h"

class GameObject3D;
class LinkedHexGrid;

struct Habitat
{
	GameObject3D* object = nullptr;
	char habitatID = 0;

	HexOffsetCoord tileCoords[3] = { {0,0}, {0,0}, {0,0} };

	// .object == nullptr means unsuccessful
	static Habitat AttemptToFormHabitat(LinkedHexGrid* hexGrid, HexOffsetCoord newHexCoords) noexcept;

private:
	void FormHabitat(LinkedHexGrid* hexGrid, int habitatIndex, HexOffsetCoord hexCoord1, HexOffsetCoord hexCoord2, HexOffsetCoord hexCoord3) noexcept;
};
