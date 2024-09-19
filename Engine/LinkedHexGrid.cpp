#include "LinkedHexGrid.h"

#include "HabitatObject.h"
#include "HexProgression.h"
#include "HexAudio.h"

#include "RandomGen.h"

#include "Debug.h"

void LinkedHexGrid::SerialiseTo(json& jsonObj) const
{
	GameObject3D::SerialiseTo(jsonObj);

	//...
}
void LinkedHexGrid::DeserialiseFrom(const json& jsonObj, GuidGeneration guidOptions)
{
	GameObject3D::DeserialiseFrom(jsonObj, guidOptions);

	//...
}
void LinkedHexGrid::UpdateFrom(const json& jsonObj, GuidGeneration guidOptions)
{
	GameObject3D::UpdateFrom(jsonObj, guidOptions);

	//...
}

HexTile& HexColumn::operator[](short row) noexcept
{
	return tiles[row];
}
const HexTile& HexColumn::operator[](short row) const noexcept
{
	return tiles[row];
}

HexTile& LinkedHexGrid::Get(HexOffsetCoord hexCoord) noexcept
{
	return tiles[hexCoord.x][hexCoord.y];
}

const HexTile& LinkedHexGrid::Get(HexOffsetCoord hexCoord) const noexcept
{
	return tiles[hexCoord.x][hexCoord.y];
}

HexTile& LinkedHexGrid::operator[](HexOffsetCoord hexCoord) noexcept
{
	return Get(hexCoord);
}

const HexTile& LinkedHexGrid::operator[](HexOffsetCoord hexCoord) const noexcept
{
	return Get(hexCoord);
}

void LinkedHexGrid::Initialise() noexcept
{
	GameObject3D::Initialise();
}

void LinkedHexGrid::UpdateTile(vec3 position, json tilePrefab) noexcept
{
	HexCubeCoord cubeCoord = HexCubeCoord::GetFromPos(vec2(position.x, position.z));
	if (cubeCoord.GetMagnitude() > radius)
	{
		Debug::LogWarning("The given position is outside the hex grid.", locationinfo);
		return;
	}

	UpdateTile(HexCubeToOffset(cubeCoord, centre), tilePrefab);
}
void LinkedHexGrid::UpdateTile(HexOffsetCoord hexCoord, json tilePrefab) noexcept
{
	HexTile& hexTile = Get(hexCoord);
	switch (hexTile.type)
	{
	case HexType::Unreached:
	{
		Debug::LogWarning(
			"Attempted to update a tile that hasn't been reached ",
			"(", hexCoord.x, ",", hexCoord.y, ")"
		);
		return;
	}
	case HexType::Perimeter:
	{
		const HexOffsetCoord neighbourCoords[6]{
			// This order is for memory alignment, columns are contiguous in the grid
			HexOffsetCoord::GetCoords(hexCoord, HexDirection::NorthWest),
			HexOffsetCoord::GetCoords(hexCoord, HexDirection::SouthWest),
			HexOffsetCoord::GetCoords(hexCoord, HexDirection::North),
			HexOffsetCoord::GetCoords(hexCoord, HexDirection::South),
			HexOffsetCoord::GetCoords(hexCoord, HexDirection::NorthEast),
			HexOffsetCoord::GetCoords(hexCoord, HexDirection::SouthEast),
		};
		for (int i = 0; i < 6; i++)
		{
			if (Get(neighbourCoords[i]).type == HexType::Unreached &&
				neighbourCoords[i].GetMagnitude(centre) <= HexProgression::currentRadius)
			{
				InitialiseTile(hexCoord);
			}
		}
		short oldRadius = (short)HexProgression::currentRadius;
		const HexProgression::Milestone* milestone = HexProgression::IncreaseLife(HexProgression::tileLifeBonus);
		if (milestone != nullptr)
		{
			// TODO: Play MilestoneReached SFX
			ValidatePerimeterPlaceability(oldRadius + 1);
		}
		break;
	}
	default: // tile already exists
	{
		if (hexTile.type == (HexType)(int)tilePrefab["HexType"] &&
			TileData::GetVariantsOfType(hexTile.type)[hexTile.variant].name == (string)tilePrefab["HexVariant"])
		{
			hexTile.object->Rotate(glm::radians(-60.0f), vec3(0, 1, 0));
			return;
		}

		TileData::GetVariantsOfType(hexTile.type)[hexTile.variant].countPlaced--;
		break;
	}
	}

	
	vec3 position = hexTile.object->GetPosition();
	hexTile.object->UpdateFrom(tilePrefab, GuidGeneration::Keep);
	hexTile.object->SetPosition(position);

	hexTile.object->Rotate(glm::radians(Random::Int32(0, 5) * 60.0f), vec3(0, 1, 0));

	hexTile.object->SetName(TileData::GetVariantsOfType(hexTile.type)[hexTile.variant].name.c_str());

	TileData::GetVariantsOfType(hexTile.type)[hexTile.variant].countPlaced++;

	HabitatObject* habitat = HabitatObject::AttemptToFormHabitat(hexTile.object);
	if (habitat != nullptr)
	{
		HexAudio::PlayMiscSFX(HexAudio::SoundEffect::FormHabitat);

		habitat->SetParent(this);
		//habitats.push_back(habitat);
		short oldRadius = (short)HexProgression::currentRadius;
		const HexProgression::Milestone* milestone = HexProgression::IncreaseLife(HexProgression::habitatLifeBonus);
		if (milestone != nullptr)
		{
			// TODO: Play MilestoneReached SFX
			ValidatePerimeterPlaceability(oldRadius + 1);
		}
	}
	else
	{
		switch (hexTile.type)
		{
		case HexType::Tree: HexAudio::PlayMiscSFX(HexAudio::SoundEffect::PlaceTree); break;
		case HexType::Flower: HexAudio::PlayMiscSFX(HexAudio::SoundEffect::PlaceFlower); break;
		case HexType::Water: HexAudio::PlayMiscSFX(HexAudio::SoundEffect::PlaceWater); break;
		case HexType::Land: HexAudio::PlayMiscSFX(HexAudio::SoundEffect::PlaceLand); break;
		default: break;
		}
	}
}

void LinkedHexGrid::InitialiseCentre() noexcept
{
	InitialiseTile(centre);
	UpdateTile(centre, HexTileObject::GetDefaultTilePrefab()); // TODO: Replace with Mother Tree
}

void LinkedHexGrid::InitialiseTile(HexOffsetCoord hexCoord) noexcept
{
	HexTile& hexTile = Get(hexCoord);

	if (hexTile.type != HexType::Unreached)
	{
		Debug::LogWarning("Attempted to initialise a tile that is already initialised.", locationinfo);
		return;
	}

	json perimeterPrefab = HexTileObject::GetEmptyTilePrefab();
	const unsigned long long typeID = perimeterPrefab["TypeID"];
	if (typeID != HexTileObject::classID)
	{
		Debug::LogError("TypeID ", typeID, " is not valid for initialising a HexTile", locationinfo);
		return;
	}

	hexTile.type = HexType::Perimeter;
	hexTile.object = (HexTileObject*)GameObject::InstantiateFrom(perimeterPrefab, GuidGeneration::New);
	gameObjectManager->Add(hexTile.object);
	hexTile.object->SetParent(this);
	vec2 position = HexOffsetCoord::ToPos(hexCoord, centre);
	hexTile.object->SetPosition(vec3(position.x, 0, position.y));
}

static void ValidateEdgePlaceability(LinkedHexGrid& hexGrid, short radius, HexCubeCoord offsetDir, HexCubeCoord moveDir, HexCubeCoord lookupDirA, HexCubeCoord lookupDirB) noexcept
{
	HexCubeCoord tileCoord(offsetDir.q * radius, offsetDir.r * radius);

	for (short i = 0; i < radius; i++)
	{
		HexTile& currentTile = hexGrid.Get(HexCubeToOffset(tileCoord, hexGrid.centre));
		const HexTile& lookupTileA = hexGrid.Get(HexCubeToOffset(tileCoord + lookupDirA, hexGrid.centre));
		const HexTile& lookupTileB = hexGrid.Get(HexCubeToOffset(tileCoord + lookupDirB, hexGrid.centre));

		if ((char)lookupTileA.type > 0 || (char)lookupTileB.type > 0)
		{
			hexGrid.InitialiseTile(HexCubeToOffset(tileCoord, hexGrid.centre));
		}

		tileCoord += moveDir;
	}
}
void LinkedHexGrid::ValidatePerimeterPlaceability(short perimeterRadius) noexcept
{
	ValidateEdgePlaceability(*this, perimeterRadius, {  0, -1 }, { +1,  0 }, { -1, +1 }, {  0, +1 }); // From North to NorthEast
	ValidateEdgePlaceability(*this, perimeterRadius, { +1, -1 }, {  0, +1 }, { -1,  0 }, { -1, +1 }); // From NorthEast to SouthEast
	ValidateEdgePlaceability(*this, perimeterRadius, { +1,  0 }, { -1, +1 }, {  0, -1 }, { -1,  0 }); // From SouthEast to South
	ValidateEdgePlaceability(*this, perimeterRadius, {  0, +1 }, { -1,  0 }, { +1, -1 }, {  0, -1 }); // From South to SouthWest
	ValidateEdgePlaceability(*this, perimeterRadius, { -1, +1 }, {  0, -1 }, { +1,  0 }, { +1, -1 }); // From SouthWest to NorthWest
	ValidateEdgePlaceability(*this, perimeterRadius, { -1,  0 }, { +1, -1 }, {  0, +1 }, { +1,  0 }); // From NorthWest to North
}
