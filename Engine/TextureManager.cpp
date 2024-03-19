#include "TextureManager.h"

TextureManager* textureManager;

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
      return nullptr;
   }
}
