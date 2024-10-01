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
   missingTexture->Create(4, 4, false, false, Texture::Filter::None, Texture::Format::RGB, missing);
   missingTexture->filepath = "Missing";
   loadedTextures.insert(std::pair(string("Missing"), missingTexture));

   Texture* defaultColour = new Texture;
   unsigned char white[3] = { 255, 255, 255 };
   defaultColour->Create(1, 1, false, false, Texture::Filter::None, Texture::Format::RGB, white);
   defaultColour->filepath = "DefaultColour";
   loadedTextures.insert(std::pair(string("DefaultColour"), defaultColour));

   Texture* defaultNormal = new Texture;
   unsigned char cornflower[3] = { 127, 127, 255 };
   defaultNormal->Create(1, 1, false, false, Texture::Filter::None, Texture::Format::RGB, cornflower);
   defaultNormal->filepath = "DefaultNormal";
   loadedTextures.insert(std::pair(string("DefaultNormal"), defaultNormal));

   Texture* defaultRMAO = new Texture;
   unsigned char rmao[3] = { 127, 0, 255 };
   defaultRMAO->Create(1, 1, false, false, Texture::Filter::None, Texture::Format::RGB, rmao);
   defaultRMAO->filepath = "DefaultRMAO";
   loadedTextures.insert(std::pair(string("DefaultRMAO"), defaultRMAO));

   defaultTextures.push_back("DefaultColour");
   defaultTextures.push_back("DefaultNormal");
   defaultTextures.push_back("DefaultRMAO");
}

TextureManager::~TextureManager()
{
   for (pair<string, Texture*> texturePair : loadedTextures) { del(texturePair.second); }
}

Texture* TextureManager::GetTexture(const char* filepath, bool isNonlinear)
{
   return GetTexture(filepath, isNonlinear, true, Texture::Filter::Trilinear);
}

Texture* TextureManager::GetTexture(const char* filepath, bool isNonlinear, bool wrap, Texture::Filter filter)
{
   if (filepath == nullptr) return nullptr;

   string filepathStr = filepath;

   if (loadedTextures.count(filepathStr) == 0) return AddTexture(filepath, isNonlinear, wrap, filter);
   else return loadedTextures[filepathStr];
}

void TextureManager::ReloadTexture(const char* filepath)
{
   if (loadedTextures.count(filepath) != 0)
   {
      loadedTextures[filepath]->Reload();
   }
}

void TextureManager::ReloadAll()
{
   for (auto& texturePair : loadedTextures)
   {
      texturePair.second->Reload();
   }
}

Texture* TextureManager::AddTexture(const char* filepath, bool isNonlinear, bool wrap, Texture::Filter filter)
{
   Texture* newTexture = new Texture;

   if (newTexture->Load(filepath, isNonlinear, wrap, filter))
   {
      loadedTextures.insert(std::pair(string(filepath), newTexture));
      return newTexture;
   }
   else
   {
      delete newTexture;
      Debug::LogWarning(LogID::WRN100, filepath, ". Set to Missing Texture", locationinfo);
      return GetTexture("Missing");
   }
}
