#include "TextureManager.h"
#include "Debug.h"

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
   missingTexture->fileName = "Missing";
   loadedTextures.insert(std::pair(string("Missing"), missingTexture));

   Texture* defaultColour = new Texture;
   unsigned char white[3] = { 255, 255, 255 };
   defaultColour->Create(1, 1, Texture::RGB, white);
   defaultColour->fileName = "DefaultColour";
   loadedTextures.insert(std::pair(string("DefaultColour"), defaultColour));

   Texture* defaultNormal = new Texture;
   unsigned char cornflower[3] = { 127, 127, 255 };
   defaultNormal->Create(1, 1, Texture::RGB, cornflower);
   defaultNormal->fileName = "DefaultNormal";
   loadedTextures.insert(std::pair(string("DefaultNormal"), defaultNormal));

   Texture* defaultRMAO = new Texture;
   unsigned char rmao[3] = { 127, 0, 255 };
   defaultRMAO->Create(1, 1, Texture::RGB, rmao);
   defaultRMAO->fileName = "DefaultRMAO";
   loadedTextures.insert(std::pair(string("DefaultRMAO"), defaultRMAO));
}

TextureManager::~TextureManager()
{
   for (pair<string, Texture*> texturePair : loadedTextures) { del(texturePair.second); }
}

Texture* TextureManager::GetTexture(const char* filepath, Texture::Linearity linearity)
{
   if (loadedTextures.count(filepath) == 0) return AddTexture(filepath, linearity);
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

Texture* TextureManager::AddTexture(const char* filepath, Texture::Linearity linearity)
{
   Texture* newTexture = new Texture;

   if (newTexture->Load(filepath, linearity))
   {
      loadedTextures.insert(std::pair(string(filepath), newTexture));
      return newTexture;
   }
   else
   {
      delete newTexture;
      Debug::Log(Debug::WRN( "Failed to load a Texture from filepath: ",
                     filepath, ". Set to Missing Texture" locationinfo ),
                     Debug::WRN100);
      return GetTexture("Missing");
   }
}
