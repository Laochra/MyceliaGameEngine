#include "UISprite.h"

#include "Shader.h"
#include "Mesh.h"

#include "AppInfo.h"

Mesh* UISprite::spriteMesh = nullptr;
ShaderProgram* UISprite::shaderProgram = nullptr;

UISprite::UISprite() noexcept :
	texture(Texture()),
	anchor({0,0}),
	offset({0,0}),
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
UISprite::UISprite(const char* spriteFile, vec2 anchorInit, vec2 offsetInit, int layerInit, float scaleInit) noexcept :
	anchor(anchorInit),
	offset(offsetInit),
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
	
	jsonObj["AnchorX"] = anchor.x;
	jsonObj["AnchorY"] = anchor.y;
	jsonObj["OffsetX"] = offset.x;
	jsonObj["OffsetY"] = offset.y;
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

	anchor = vec2(float(jsonObj["AnchorX"]), float(jsonObj["AnchorY"]));
	offset = vec2(float(jsonObj["OffsetX"]), float(jsonObj["OffsetY"]));
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

	float spriteAspect = (float)texture.GetWidth() / (float)texture.GetHeight();
	float screenAspect = 1.0f / (AppInfo::screenHeight == 0 ? 0.0f : (AppInfo::screenWidth / (float)AppInfo::screenHeight));
	vec2 spritePos = vec2(anchor.x + offset.x * screenAspect, anchor.y + offset.y);
	
	shaderProgram->BindUniform("SpritePos", spritePos);
	shaderProgram->BindUniform("Aspect", spriteAspect * screenAspect);
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
