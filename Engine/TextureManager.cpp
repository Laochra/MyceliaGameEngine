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
   loadedTextures.insert(std::pair("Missing", missingTexture));

   Texture* defaultColour = new Texture;
   unsigned char white[3] = { 255, 255, 255 };
   defaultColour->Create(1, 1, false, false, Texture::Filter::None, Texture::Format::RGB, white);
   defaultColour->filepath = "DefaultColour";
   loadedTextures.insert(std::pair("DefaultColour", defaultColour));

   Texture* defaultNormal = new Texture;
   unsigned char cornflower[3] = { 127, 127, 255 };
   defaultNormal->Create(1, 1, false, false, Texture::Filter::None, Texture::Format::RGB, cornflower);
   defaultNormal->filepath = "DefaultNormal";
   loadedTextures.insert(std::pair("DefaultNormal", defaultNormal));

   Texture* defaultRMAO = new Texture;
   unsigned char rmao[3] = { 127, 0, 255 };
   defaultRMAO->Create(1, 1, false, false, Texture::Filter::None, Texture::Format::RGB, rmao);
   defaultRMAO->filepath = "DefaultRMAO";
   loadedTextures.insert(std::pair("DefaultRMAO", defaultRMAO));

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
   if (filepath == nullptr) return nullptr;

   return GetTexture(string(filepath), isNonlinear);
}
Texture* TextureManager::GetTexture(const string& filepath, bool isNonlinear)
{
   return GetTexture(filepath, isNonlinear, true, Texture::Filter::Trilinear);
}
Texture* TextureManager::GetTexture(const char* filepath, bool isNonlinear, bool wrap, Texture::Filter filter)
{
   if (filepath == nullptr) return nullptr;

   return GetTexture(string(filepath), isNonlinear, wrap, filter);
}
Texture* TextureManager::GetTexture(const string& filepath, bool isNonlinear, bool wrap, Texture::Filter filter)
{
   if (filepath == "None") return nullptr;

   auto it = loadedTextures.find(filepath);

   if (it == loadedTextures.end())
   {
      it = loadedTextures.find(filepath);
      return AddTexture(filepath, isNonlinear, wrap, filter);
   }
   else
   {
      return it->second;
   }
}

void TextureManager::ReloadTexture(const string& filepath)
{
   auto it = loadedTextures.find(filepath);

   if (it != loadedTextures.end())
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

Texture* TextureManager::AddTexture(const string& filepath, bool isNonlinear, bool wrap, Texture::Filter filter)
{
   Texture* newTexture = new Texture;

   if (newTexture->Load(filepath.c_str(), isNonlinear, wrap, filter))
   {
      loadedTextures.insert(std::pair(filepath, newTexture));
      return newTexture;
   }
   else
   {
      delete newTexture;
      Debug::LogWarning(LogID::WRN100, filepath, ". Set to Missing Texture", locationinfo);
      return GetTexture("Missing");
   }
}
