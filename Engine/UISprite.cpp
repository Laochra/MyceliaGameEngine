#include "UISprite.h"

#include "Shader.h"
#include "Mesh.h"

#include "AppInfo.h"

Mesh* UISprite::spriteMesh = nullptr;
ShaderProgram* UISprite::shaderProgram = nullptr;

UISprite::UISprite() noexcept :
	texture(Texture()),
	normalisedPosition({0,0}),
	layer(0),
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
UISprite::UISprite(const char* spriteFile, vec2 normalisedPositionInit, int layerInit, float scaleInit) noexcept :
	normalisedPosition(normalisedPositionInit),
	layer(layerInit),
	scale(scaleInit),
	enabled(true),
	name("UISprite")
{
	texture.Load(spriteFile, false, false, Texture::Filter::None);

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
	texture.Load(spriteFile, false, false, Texture::Filter::None);
}

void UISprite::SerialiseTo(json& jsonObj) const noexcept
{
	jsonObj["Name"] = name;
	jsonObj["Enabled"] = enabled;
	
	jsonObj["PositionX"] = normalisedPosition.x;
	jsonObj["PositionY"] = normalisedPosition.y;
	jsonObj["Layer"] = layer;
	jsonObj["Scale"] = scale;

	jsonObj["Texture"] = texture.GetFilepath();
	jsonObj["TextureFilter"] = texture.GetFilter();
	jsonObj["TextureWrap"] = texture.GetWrap();
}
void UISprite::DeserialiseFrom(const json& jsonObj) noexcept
{
	name = jsonObj["Name"];
	enabled = jsonObj["Enabled"];

	normalisedPosition = vec2(float(jsonObj["PositionX"]), float(jsonObj["PositionY"]));
	if (jsonObj.contains("Layer")) layer = jsonObj["Layer"];
	scale = jsonObj["Scale"];

	string filepath((string)jsonObj["Texture"]);
	Texture::Filter filter(Texture::Filter::None);
	bool wrap = false;
	if (jsonObj.contains("TextureFilter")) filter = jsonObj["TextureFilter"];
	if (jsonObj.contains("TextureWrap")) wrap = jsonObj["TextureWrap"];

	texture.Load(filepath.c_str(), false, wrap, filter);
}

void UISprite::Draw() const noexcept
{
	if (!enabled || texture.GetFilepath() == "None") return;

	shaderProgram->Bind();

	shaderProgram->BindUniform("SpritePos", normalisedPosition);
	float spriteAspect = (float)texture.GetWidth() / (float)texture.GetHeight();
	shaderProgram->BindUniform("SpriteAspect", spriteAspect);
	float screenAspect = 1.0f / (AppInfo::screenHeight == 0 ? 0.0f : (AppInfo::screenWidth / (float)AppInfo::screenHeight));
	shaderProgram->BindUniform("ScreenAspect", screenAspect);
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
	return texture.GetFilepath();
}
