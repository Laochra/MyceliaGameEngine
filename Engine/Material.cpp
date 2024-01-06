#include "Material.h"

Texture Material::GetTexture() const noexcept
{
    return *texture;
}

const char* Material::GetTextureFilename() const noexcept
{
    return texture->GetFileName().c_str();
}