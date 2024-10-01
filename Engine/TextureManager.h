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

	Texture* GetTexture(const char* filepath, bool isNonlinear = false);
	Texture* GetTexture(const char* filepath, bool isNonlinear, bool wrap, Texture::Filter filter);
	
	void ReloadTexture(const char* filepath);
	void ReloadAll();

private:
	map<string, Texture*> loadedTextures = map<string, Texture*>();

	Texture* AddTexture(const char* filepath, bool isNonlinear, bool wrap, Texture::Filter filter);
};

extern TextureManager* textureManager;