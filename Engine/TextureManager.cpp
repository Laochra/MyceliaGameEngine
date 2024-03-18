#include "TextureManager.h"

Texture* TextureManager::GetTexture(const char* filepath)
{
   if (loadedTextures.count(filepath) == 0) return AddTexture(filepath);
   else return loadedTextures[filepath];
}

void TextureManager::ReloadTexture(const char* filepath)
{
   if (loadedTextures.count(filepath) != 0)
   {
      delete loadedTextures[filepath];
      loadedTextures[filepath] = new Texture(filepath);
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
   else return nullptr;
}
