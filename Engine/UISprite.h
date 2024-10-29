#pragma once

#include "InspectableObject.h"

#include "Texture.h"
#include "MathIncludes.h"

#include "JsonIncludes.h"

class Mesh;
class ShaderProgram;

class UISprite : public InspectableObject
{
public:
	vec2 anchor;
	vec2 offset;
	int layer;
	float scale;
	bool enabled;
	std::string name;

	UISprite() noexcept;
	UISprite(const char* spriteFile, vec2 anchorInit = vec2(), vec2 offsetInit = vec2(), int layerInit = 0, float scaleInit = 1.0f) noexcept;
	void Load(const char* spriteFile) noexcept;

	void SerialiseTo(json& jsonObj) const noexcept;
	void DeserialiseFrom(const json& jsonObj) noexcept;

	void Draw() const noexcept;
	const char* GetName() const noexcept override;
	void SetName(const char* newName) noexcept;
	
	const std::string& GetSpritePath() const noexcept;

private:
	Texture* texture = nullptr;
	static ShaderProgram* shaderProgram;
	static Mesh* spriteMesh;
};
