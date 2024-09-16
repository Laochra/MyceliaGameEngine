#include "UISprite.h"

#include "Shader.h"
#include "Mesh.h"

#include "AppInfo.h"

Mesh* UISprite::spriteMesh = nullptr;
ShaderProgram* UISprite::shaderProgram = nullptr;

UISprite::UISprite(const char* spriteFile, vec2 normalisedPositionInit, float scaleInit) noexcept :
	texture(spriteFile, Texture::NonLinear),
	normalisedPosition(normalisedPositionInit),
	scale(scaleInit),
	enabled(true)
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

void UISprite::Draw() const noexcept
{
	if (!enabled) return;

	shaderProgram->Bind();

	shaderProgram->BindUniform("SpritePos", normalisedPosition);

	float aspect = 1.0f / (AppInfo::screenHeight == 0 ? 0.0f : (AppInfo::screenWidth / (float)AppInfo::screenHeight));
	shaderProgram->BindUniform("Aspect", aspect);

	shaderProgram->BindUniform("Scale", scale);

	texture.Bind(0);
	shaderProgram->BindUniform("SpriteTexture", 0);

	spriteMesh->Draw();
}
