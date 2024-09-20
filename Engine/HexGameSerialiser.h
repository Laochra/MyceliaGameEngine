#pragma once

#include "JsonIncludes.h"

class HexGameSerialiser
{
public:
	static void LoadDataFrom(json& dataFile) noexcept;
	static void SaveDataTo(json& dataFile) noexcept;
};
