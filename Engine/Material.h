#pragma once

#include "Texture.h"
#include "Shader.h"

class Material
{
public:
	Texture GetTexture() const noexcept;
	const char* GetTextureFilename() const noexcept;

private:
	Texture* texture = nullptr;
	ShaderProgram shaderProgram = ShaderProgram();
};