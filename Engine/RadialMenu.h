#pragma once

#include "GLIncludes.h"
#include "Shader.h"

#include "Mesh.h"

class RadialMenu
{
public:
	float scale = 0.75f;
	float deadzoneMagnitudeSqr = 0.15f * 0.15f;
	unsigned int sliceCount = 3;

	RadialMenu() noexcept;
	~RadialMenu() noexcept;

	void Draw(vec2 input) noexcept;

private:
	ShaderProgram program;
	unsigned int spriteArray;
	Mesh spriteMesh;
};
