#pragma once

#include "GLIncludes.h"
#include "Shader.h"

#include "Mesh.h"

#include "Input.h"

typedef unsigned short ushort;

class RadialMenu
{
public:
	typedef void (*InteractFunc)(int selection) noexcept;

	float scale = 0.75f;
	float deadzoneMagnitudeSqr = 0.15f * 0.15f;
	vec2 lastInput;
	InteractFunc interactionHandler;
	ushort radialSlices = 3;
	vector<bool> sliceEnabledFlags = { true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true };
	bool enabled = false;
	bool initialInputGiven = false;

	RadialMenu(const char* regularSprite, const char* hoveredSprite, const char* disabledSprite) noexcept;
	~RadialMenu() noexcept;

	void Update(vec2 input, InputBind interactKey) noexcept;
	void Draw() noexcept;

private:
	ShaderProgram program;
	unsigned int spriteArray;
	Mesh spriteMesh;
};
