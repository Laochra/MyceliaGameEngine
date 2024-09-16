#pragma once

#include "Texture.h"
#include "MathIncludes.h"
class Mesh;
class ShaderProgram;

class UISprite
{
public:
	vec2 normalisedPosition;
	float scale;
	bool enabled;

	UISprite(const char* spriteFile, vec2 normalisedPositionInit = vec2(), float scaleInit = 1.0f) noexcept;

	void Draw() const noexcept;

private:
	Texture texture;
	static ShaderProgram* shaderProgram;
	static Mesh* spriteMesh;
};
