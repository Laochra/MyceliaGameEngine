#pragma once

#include "Texture.h"

#include <map>
using std::map;

#include <string>
using std::string;

#include <vector>
using std::vector;

#include "GeneralMacros.h"

class TextureManager
{
public:
	vector<string> defaultTextures;

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