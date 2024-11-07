#include "HexGrid.h"

#include "TileData.h"
#include "SpiritData.h"

#include "HexProgression.h"
#include "HexAudio.h"

#include "RandomGen.h"

#include "Debug.h"

void HexGrid::SerialiseTo(json& jsonObj) const
{
	GameObject3D::SerialiseTo(jsonObj);

	//...
}
void HexGrid::DeserialiseFrom(const json& jsonObj, GuidGeneration guidOptions)
{
	GameObject3D::DeserialiseFrom(jsonObj, guidOptions);

	//...
}
bool HexGrid::UpdateFrom(const json& jsonObj, GuidGeneration guidOptions)
{
	if (!GameObject3D::UpdateFrom(jsonObj, guidOptions)) return false;

	//...

	return true;
}

HexTile& HexColumn::operator[](short row) noexcept
{
	return tiles[row];
}
const HexTile& HexColumn::operator[](short row) const noexcept
{
	return tiles[row];
}

HexTile& HexGrid::Get(HexOffsetCoord hexCoord) noexcept
{
	return tiles[hexCoord.x][hexCoord.y];
}

const HexTile& HexGrid::Get(HexOffsetCoord hexCoord) const noexcept
{
	return tiles[hexCoord.x][hexCoord.y];
}

HexTile& HexGrid::operator[](HexOffsetCoord hexCoord) noexcept
{
	return Get(hexCoord);
}

const HexTile& HexGrid::operator[](HexOffsetCoord hexCoord) const noexcept
{
	return Get(hexCoord);
}

void HexGrid::Initialise() noexcept
{
	GameObject3D::Initialise();
}

HexGrid::UpdateTileReturnInfo HexGrid::UpdateTile(vec3 position, const json& tilePrefab) noexcept
{
	HexCubeCoord cubeCoord = HexCubeCoord::GetFromPos(vec2(position.x, position.z));
	if (cubeCoord.GetMagnitude() > radius) return UpdateTileReturnInfo();

	return UpdateTile(HexCubeToOffset(cubeCoord, centre), tilePrefab);
}
HexGrid::UpdateTileReturnInfo HexGrid::UpdateTile(HexOffsetCoord hexCoord, const json& tilePrefab) noexcept
{
	UpdateTileReturnInfo returnInfo;

	if (hexCoord == centre && tilePrefab != *TileData::GetMotherTreePrefab()) return returnInfo;

	HexTile& hexTile = Get(hexCoord);
	
	if (hexTile.habitat >= 0)
	{
		returnInfo.value |= UpdateTileReturnInfo::HabitatPickedUp;
		returnInfo.habitatID = hexTile.habitat;
		return returnInfo;
	}

	short oldRadius = (short)HexProgression::currentRadius;

	switch (hexTile.type)
	{
	case HexType::Unreached:
	{
		return returnInfo;
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
				neighbourCoords[i].GetMagnitude(centre) <= (short)HexProgression::currentRadius)
			{
				InitialiseTile(neighbourCoords[i]);
			}
		}
		const HexProgression::Milestone* milestone = HexProgression::IncreaseLife(HexProgression::tileLifeBonus);
		if (milestone != nullptr)
		{
			returnInfo.value |= UpdateTileReturnInfo::MilestoneReached;
		}

		returnInfo.value |= UpdateTileReturnInfo::TilePlaced | UpdateTileReturnInfo::NewTile;
		break;
	}
	default: // tile already exists
	{
		if ((int)hexTile.type == tilePrefab["HexType"] &&
			TileData::Get(hexTile.type)[hexTile.variant].name == (string)tilePrefab["HexVariant"])
		{
			hexTile.object->Rotate(glm::radians(-60.0f), vec3(0, 1, 0));
			return returnInfo;
		}
		if ((char)hexTile.type >= 0)
		{
			TileData::Get(hexTile.type)[hexTile.variant].countPlaced--;
		}

		returnInfo.value |= UpdateTileReturnInfo::TilePlaced;
		break;
	}
	}
	
	vec2 position = vec2(hexTile.object->GetPosition().x, hexTile.object->GetPosition().z);
	unsigned long long guid = hexTile.object->GetGUID();
	GameObject::Destroy(hexTile.object);
	hexTile.object = (GameObject3D*)GameObject::InstantiateFrom(tilePrefab, GuidGeneration::New);
	gameObjectManager->Add(hexTile.object);
	hexTile.object->SetParent(this);
	hexTile.object->SetPosition(vec3(position.x, hexTile.object->GetPosition().y, position.y));
	hexTile.object->Rotate(glm::radians(Random::Int32(0, 5) * 60.0f), vec3(0, 1, 0));

	hexTile.type = tilePrefab["HexType"];
	if ((char)hexTile.type >= 0)
	{
		hexTile.variant = TileData::GetVariantIndex(hexTile.type, tilePrefab["HexVariant"]);
		hexTile.object->SetName(TileData::Get(hexTile.type)[hexTile.variant].name.c_str());
		TileData::Get(hexTile.type)[hexTile.variant].countPlaced++;
	}

	Habitat habitat = Habitat::AttemptToFormHabitat(this, hexCoord);
	if (habitat.object != nullptr)
	{
		returnInfo.value |= UpdateTileReturnInfo::NewHabitat;
		returnInfo.habitatID = habitat.habitatID;

		habitat.object->SetParent(this);
		habitats.push_back(habitat);
		const HexProgression::Milestone* milestone = HexProgression::IncreaseLife(HexProgression::habitatLifeBonus);
		if (milestone != nullptr)
		{
			returnInfo.value |= UpdateTileReturnInfo::MilestoneReached;
		}

		Spirit spirit;
		spirit.habitatID = habitat.habitatID;
		spirit.object = (GameObject3D*)GameObject::InstantiateFrom(*SpiritData::GetPrefab(habitat.habitatID));
		spirit.object->SetParent(habitat.object);
		spirits.push_back(spirit);
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

	if (returnInfo.value & UpdateTileReturnInfo::NewHabitat)
	{
		ValidatePerimeterPlaceability(oldRadius + 1);
	}

	return returnInfo;
}

void HexGrid::InitialiseCentre() noexcept
{
	InitialiseTile(centre);
	UpdateTile(centre, *TileData::GetMotherTreePrefab()); // TODO: Replace with Mother Tree
}

void HexGrid::InitialiseTile(HexOffsetCoord hexCoord) noexcept
{
	HexTile& hexTile = Get(hexCoord);

	if (hexTile.type != HexType::Unreached)
	{
		Debug::LogWarning("Attempted to initialise a tile that is already initialised.", locationinfo);
		return;
	}

	json perimeterPrefab = *TileData::GetPerimeterPrefab();

	hexTile.type = HexType::Perimeter;
	hexTile.object = (GameObject3D*)GameObject::InstantiateFrom(perimeterPrefab, GuidGeneration::New);
	gameObjectManager->Add(hexTile.object);
	hexTile.object->SetParent(this);
	vec2 position = HexOffsetCoord::ToPos(hexCoord, centre);
	hexTile.object->SetPosition(vec3(position.x, 0, position.y));
}

static void ValidateEdgePlaceability(HexGrid& hexGrid, short radius, HexCubeCoord offsetDir, HexCubeCoord moveDir, HexCubeCoord lookupDirA, HexCubeCoord lookupDirB) noexcept
{
	HexCubeCoord tileCoord(offsetDir.q * radius, offsetDir.r * radius);

	for (short i = 0; i < radius; i++)
	{
		HexTile& currentTile = hexGrid.Get(HexCubeToOffset(tileCoord, hexGrid.centre));
		const HexTile& lookupTileA = hexGrid.Get(HexCubeToOffset(tileCoord + lookupDirA, hexGrid.centre));
		const HexTile& lookupTileB = hexGrid.Get(HexCubeToOffset(tileCoord + lookupDirB, hexGrid.centre));

		if ((char)lookupTileA.type >= 0 || (char)lookupTileB.type >= 0)
		{
			hexGrid.InitialiseTile(HexCubeToOffset(tileCoord, hexGrid.centre));
		}

		tileCoord += moveDir;
	}
}
void HexGrid::ValidatePerimeterPlaceability(short perimeterRadius) noexcept
{
	ValidateEdgePlaceability(*this, perimeterRadius, {  0, -1 }, { +1,  0 }, { -1, +1 }, {  0, +1 }); // From North to NorthEast
	ValidateEdgePlaceability(*this, perimeterRadius, { +1, -1 }, {  0, +1 }, { -1,  0 }, { -1, +1 }); // From NorthEast to SouthEast
	ValidateEdgePlaceability(*this, perimeterRadius, { +1,  0 }, { -1, +1 }, {  0, -1 }, { -1,  0 }); // From SouthEast to South
	ValidateEdgePlaceability(*this, perimeterRadius, {  0, +1 }, { -1,  0 }, { +1, -1 }, {  0, -1 }); // From South to SouthWest
	ValidateEdgePlaceability(*this, perimeterRadius, { -1, +1 }, {  0, -1 }, { +1,  0 }, { +1, -1 }); // From SouthWest to NorthWest
	ValidateEdgePlaceability(*this, perimeterRadius, { -1,  0 }, { +1, -1 }, {  0, +1 }, { +1,  0 }); // From NorthWest to North
}
