#pragma once

#include "GLIncludes.h"
#include "Shader.h"

#include "Mesh.h"

#include "Input.h"


class RadialMenu
{
public:
	typedef void (*InteractFunc)(uint selection) noexcept;

	float scale = 0.75f;
	float deadzoneMagnitudeSqr = 0.15f * 0.15f;
	vec2 lastInput;
	InteractFunc interactionHandler;
	uint radialSlices = 3;
	bool enabled = false;

	RadialMenu(const char* activeSprite, const char* inactiveSprite, const char* disabledSprite) noexcept;
	~RadialMenu() noexcept;

	void Update(vec2 input, InputBind interactKey) noexcept;
	void Draw() noexcept;

private:
	ShaderProgram program;
	unsigned int spriteArray;
	Mesh spriteMesh;
};
