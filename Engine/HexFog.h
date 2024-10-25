#pragma once

typedef unsigned int uint;

class HexFog
{
public:
	static uint leakingFogImage;
	static uint fullFogImage;

	static void MakeHexagonalDistanceField() noexcept;
	static void MakeHexagonImage() noexcept;
};
