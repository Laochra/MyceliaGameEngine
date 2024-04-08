#pragma once

#include "Texture.h"

#include <map>
using std::map;

#include <string>
using std::string;

#include "GeneralMacros.h"

class TextureManager
{
public:
	TextureManager();
	~TextureManager();
	uncopyable(TextureManager);
	immovable(TextureManager);

	Texture* GetTexture(const char* filepath, Texture::Linearity linearity = Texture::Linear);
	void ReloadTexture(const char* filepath);

private:
	map<string, Texture*> loadedTextures = map<string, Texture*>();

	Texture* AddTexture(const char* filepath, Texture::Linearity linearity = Texture::Linear);
};

extern TextureManager* textureManager;