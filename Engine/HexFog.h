#pragma once

#include "Texture.h"

typedef unsigned int uint;

class HexFog
{
public:
	static float currentRadius;
	static float gradientRange;
	static float animationSpeed;

	static std::string fogTextureFilepath;

	static void AnimateFogTo(float newRadius) noexcept;

	// Expects a HDR Image
	static void Load(Texture::Filter filterInit) noexcept;

	static void Bind(int slot) noexcept;

	static const Texture::Filter GetFilter() noexcept;

	static void MakeHexagonalDistanceField() noexcept;
	static void MakeHexagonImage() noexcept;

	friend class HexGameSerialiser;

private:
	static uint fogDistanceField;
	static Texture::Filter filter;
};
