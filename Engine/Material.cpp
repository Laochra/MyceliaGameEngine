#include "Material.h"

Texture Material::GetTexture() const noexcept
{
    return *texture;
}

const char* Material::GetTextureFilename() const noexcept
{
    return texture->GetFileName().c_str();
}

bool Material::LoadFromJSON(const char* filename)
{
   //if (!JSONFileIsValid(filename)) return false;

   ifstream input(filename);
   json material;
   input >> material;

   material[""];

   return true;
}
