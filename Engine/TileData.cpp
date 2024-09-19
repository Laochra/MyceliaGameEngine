#include "TileData.h"

vector<vector<TileData>> TileData::data;

TileData::TileData(const char* const nameInit) noexcept :
	name(nameInit) {}

vector<TileData>& TileData::GetVariantsOfType(HexType hexType) noexcept
{
	return data[(size_t)hexType > 0 ? (size_t)hexType - 1 : 0];
}
