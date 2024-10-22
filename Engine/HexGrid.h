#pragma once

#include "HexCoords.h"
#include "HexTile.h"
#include "Habitat.h"

#include "MathIncludes.h"

#include "GameObject3D.h"

typedef unsigned int uint;
typedef unsigned long long ulong;

#define HEX_GRID_RADIUS 18

struct HexColumn
{
	HexTile tiles[HEX_GRID_RADIUS * 2 + 1];
	HexTile& operator[](short row) noexcept;
	const HexTile& operator[](short row) const noexcept;
};

// Implemented with Offset Coordinates
class HexGrid : public GameObject3D
{
public:
	SerialiseAs(HexGrid)

	using GameObject3D::GameObject3D;

	static constexpr short radius = HEX_GRID_RADIUS;
	static constexpr short columns = radius * 2 + 1;
	static constexpr short rows = radius * 2 + 1;

	static const inline HexOffsetCoord centre{ columns / 2, rows / 2 };

	HexColumn tiles[columns];
	HexTile& Get(HexOffsetCoord hexCoord) noexcept;
	const HexTile& Get(HexOffsetCoord hexCoord) const noexcept;
	HexTile& operator[](HexOffsetCoord hexCoord) noexcept;
	const HexTile& operator[](HexOffsetCoord hexCoord) const noexcept;

	vector<Habitat> habitats;

	virtual void Initialise() noexcept override;

	void InitialiseCentre() noexcept;
	void InitialiseTile(HexOffsetCoord hexCoord) noexcept;

	struct UpdateTileReturnInfo
	{
		char value = None;
		char habitatID = 0;

		typedef enum : unsigned char
		{
			None					= 0b00000000,
			TilePlaced			= 0b00000001,
			NewTile				= 0b00000010,
			NewHabitat			= 0b00000100,
			MilestoneReached	= 0b00001000,
			HabitatPickedUp	= 0b00010000,
		} bitmask;
	};
	UpdateTileReturnInfo UpdateTile(vec3 position, const json& tilePrefab) noexcept;
	UpdateTileReturnInfo UpdateTile(HexOffsetCoord hexCoord, const json& tilePrefab) noexcept;

private:
	void ValidatePerimeterPlaceability(short perimeterRadius) noexcept;
};

#undef HEX_GRID_RADIUS
