#pragma once

#include "Texture.h"

typedef unsigned int uint;

class HexFog
{
public:
	static float currentRadius;
	static float gradientRange;

	// Expects a HDR Image
	static void Load(const char* filepath, Texture::Filter filterInit) noexcept;

	static void Bind(int slot) noexcept;

	static const Texture::Filter GetFilter() noexcept;

	static void MakeHexagonalDistanceField() noexcept;
	static void MakeHexagonImage() noexcept;

private:
	static uint fogDistanceField;
	static Texture::Filter filter;
};
