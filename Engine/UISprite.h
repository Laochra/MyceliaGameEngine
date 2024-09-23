#pragma once

#include "InspectableObject.h"

#include "Texture.h"
#include "MathIncludes.h"

class Mesh;
class ShaderProgram;

class UISprite : public InspectableObject
{
public:
	vec2 normalisedPosition;
	float scale;
	bool enabled;
	std::string name;

	UISprite(const char* spriteFile, vec2 normalisedPositionInit = vec2(), float scaleInit = 1.0f) noexcept;

	void Draw() const noexcept;
	const char* GetName() const noexcept override;

private:
	Texture texture;
	static ShaderProgram* shaderProgram;
	static Mesh* spriteMesh;
};
