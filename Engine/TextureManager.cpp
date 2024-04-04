#include "TextureManager.h"

#include <iostream>

using std::pair;

TextureManager* textureManager;

TextureManager::TextureManager()
{
   Texture* missingTexture = new Texture;
   unsigned char missing[16 * 3] = {   255, 000, 255,   255, 000, 255,    000, 000, 000,    000, 000, 000,
                                       255, 000, 255,   255, 000, 255,    000, 000, 000,    000, 000, 000,
                                       000, 000, 000,   000, 000, 000,    255, 000, 255,    255, 000, 255,
                                       000, 000, 000,   000, 000, 000,    255, 000, 255,    255, 000, 255   };
   missingTexture->Create(4, 4, Texture::RGB, missing);
   loadedTextures.insert(std::pair(string("Missing"), missingTexture));

   Texture* defaultColour = new Texture;
   unsigned char white[3] = { 255, 255, 255 };
   defaultColour->Create(1, 1, Texture::RGB, white);
   loadedTextures.insert(std::pair(string("DefaultColour"), defaultColour));

   Texture* defaultNormal = new Texture;
   unsigned char cornflower[3] = { 127, 127, 255 };
   defaultNormal->Create(1, 1, Texture::RGB, cornflower);
   loadedTextures.insert(std::pair(string("DefaultNormal"), defaultNormal));
}

TextureManager::~TextureManager()
{
   for (pair<string, Texture*> texturePair : loadedTextures) { del(texturePair.second); }
}

Texture* TextureManager::GetTexture(const char* filepath)
{
   if (loadedTextures.count(filepath) == 0) return AddTexture(filepath);
   else return loadedTextures[filepath];
}

void TextureManager::ReloadTexture(const char* filepath)
{
   if (loadedTextures.count(filepath) != 0)
   {
      int glHandle = loadedTextures[filepath]->glHandle;
      delete loadedTextures[filepath];
      loadedTextures[filepath] = new Texture(filepath);
      loadedTextures[filepath]->Bind(glHandle);
   }
}

Texture* TextureManager::AddTexture(const char* filepath)
{
   Texture* newTexture = new Texture;

   if (newTexture->Load(filepath))
   {
      loadedTextures.insert(std::pair(string(filepath), newTexture));
      return newTexture;
   }
   else
   {
      delete newTexture;
      std::cout << "Couldn't find a Texture at filepath: " << filepath << ". Set to Missing Texture\n";
      return GetTexture("Missing");
   }
}
