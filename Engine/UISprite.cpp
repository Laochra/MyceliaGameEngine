#include "UISprite.h"

#include "Shader.h"
#include "Mesh.h"

#include "AppInfo.h"

Mesh* UISprite::spriteMesh = nullptr;
ShaderProgram* UISprite::shaderProgram = nullptr;

UISprite::UISprite() noexcept :
	texture(Texture()),
	normalisedPosition({0,0}),
	scale(1.0f),
	enabled(true),
	name("UISprite")
{
	if (spriteMesh == nullptr)
	{
		spriteMesh = new Mesh;
		spriteMesh->InitialiseSpriteQuad();
	}
	if (shaderProgram == nullptr)
	{
		shaderProgram = new ShaderProgram("Assets\\Shaders\\Sprite.vert", "Assets\\Shaders\\Sprite.frag");
	}
}
UISprite::UISprite(const char* spriteFile, vec2 normalisedPositionInit, float scaleInit) noexcept :
	texture(spriteFile, Texture::NonLinear),
	normalisedPosition(normalisedPositionInit),
	scale(scaleInit),
	enabled(true),
	name("UISprite")
{
	if (spriteMesh == nullptr)
	{
		spriteMesh = new Mesh;
		spriteMesh->InitialiseSpriteQuad();
	}
	if (shaderProgram == nullptr)
	{
		shaderProgram = new ShaderProgram("Assets\\Shaders\\Sprite.vert", "Assets\\Shaders\\Sprite.frag");
	}
}

void UISprite::Load(const char* spriteFile) noexcept
{
	texture.Load(spriteFile, Texture::Linear);
}

void UISprite::SerialiseTo(json& jsonObj) const noexcept
{
	jsonObj["Name"] = name;
	jsonObj["Enabled"] = enabled;
	
	jsonObj["PositionX"] = normalisedPosition.x;
	jsonObj["PositionY"] = normalisedPosition.y;
	jsonObj["Scale"] = scale;

	jsonObj["Texture"] = texture.GetFileName();
}
void UISprite::DeserialiseFrom(const json& jsonObj) noexcept
{
	name = jsonObj["Name"];
	enabled = jsonObj["Enabled"];

	normalisedPosition = vec2(float(jsonObj["PositionX"]), float(jsonObj["PositionY"]));
	scale = jsonObj["Scale"];

	texture.Load(string(jsonObj["Texture"]).c_str(), Texture::Linear);
}

void UISprite::Draw() const noexcept
{
	if (!enabled || texture.GetFileName() == "None") return;

	shaderProgram->Bind();

	shaderProgram->BindUniform("SpritePos", normalisedPosition);

	float aspect = 1.0f / (AppInfo::screenHeight == 0 ? 0.0f : (AppInfo::screenWidth / (float)AppInfo::screenHeight)) * texture.GetWidth() / texture.GetHeight();
	shaderProgram->BindUniform("Aspect", aspect);

	shaderProgram->BindUniform("Scale", scale);

	texture.Bind(0);
	shaderProgram->BindUniform("SpriteTexture", 0);

	spriteMesh->Draw();
}

const char* UISprite::GetName() const noexcept
{
	return name.c_str();
}

void UISprite::SetName(const char* newName) noexcept
{
	name = newName;
}

const std::string& UISprite::GetSpritePath() const noexcept
{
	return texture.GetFileName();
}
