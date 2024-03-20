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
	TextureManager() = default;
	~TextureManager();
	uncopyable(TextureManager);
	immovable(TextureManager);

	Texture* GetTexture(const char* filepath);
	void ReloadTexture(const char* filepath);

private:
	map<string, Texture*> loadedTextures = map<string, Texture*>();

	Texture* AddTexture(const char* filepath);
};

extern TextureManager* textureManager;