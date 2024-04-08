#include "glad.h"
#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture::Texture() : fileName("none"), width(0), height(0), glHandle(0), format(None), loadedPixels(nullptr) { }

Texture::Texture(const char* fileNameInit, Linearity linearity) : fileName("none"), width(0), height(0), glHandle(0), format(None), loadedPixels(nullptr)
{
	Load(fileNameInit, linearity);
}

Texture::Texture(unsigned int widthInit, unsigned int heightInit, Format formatInit, unsigned char* pixels, Linearity linearity) :
	fileName("none"), width(widthInit), height(heightInit), format(formatInit), loadedPixels(nullptr)
{
	Create(width, height, formatInit, pixels, linearity);
}

Texture::~Texture()
{
	if (glHandle != 0) glDeleteTextures(1, &glHandle);
	if (loadedPixels != nullptr) stbi_image_free(loadedPixels);
}

bool Texture::Load(const char* filename, Linearity linearity)
{
	if (glHandle != 0)
	{
		glDeleteTextures(1, &glHandle);
		glHandle = 0;
		width = 0;
		height = 0;
		fileName = "none";
	}

	int x = 0, y = 0, comp = 0;
	loadedPixels = stbi_load(filename, &x, &y, &comp, STBI_default);

	if (loadedPixels != nullptr)
	{
		glGenTextures(1, &glHandle);
		glBindTexture(GL_TEXTURE_2D, glHandle);
		switch (comp)
		{
		case STBI_grey: glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, x, y, 0, GL_RED, GL_UNSIGNED_BYTE, loadedPixels);
			format = SingleChannel; break;
		case STBI_grey_alpha: glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, x, y, 0, GL_RG, GL_UNSIGNED_BYTE, loadedPixels);
			format = DualChannel; break;
		case STBI_rgb: glTexImage2D(GL_TEXTURE_2D, 0, linearity ? GL_SRGB : GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, loadedPixels);
			format = RGB; break;
		case STBI_rgb_alpha: glTexImage2D(GL_TEXTURE_2D, 0, linearity ? GL_SRGB_ALPHA : GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, loadedPixels);
			format = RGBA; break;
		default:
			break;
		};
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
		width = (unsigned int)x;
		height = (unsigned int)y;
		fileName = filename;
		return true;
	}
	return false;
}

void Texture::Create(unsigned int widthInit, unsigned int heightInit, Format formatInit, unsigned char* pixels, Linearity linearity)
{
	if (glHandle != 0) {
		glDeleteTextures(1, &glHandle);
		glHandle = 0;
		fileName = "none";
	}

	width = widthInit;
	height = heightInit;
	format = formatInit;

	glGenTextures(1, &glHandle);
	glBindTexture(GL_TEXTURE_2D, glHandle);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	switch (format)
	{
	case SingleChannel: glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, pixels);
		break;
	case DualChannel: glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, width, height, 0, GL_RG, GL_UNSIGNED_BYTE, pixels);
		break;
	case RGB: glTexImage2D(GL_TEXTURE_2D, 0, linearity ? GL_SRGB : GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
		break;
	case RGBA: glTexImage2D(GL_TEXTURE_2D, 0, linearity ? GL_SRGB_ALPHA : GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
		break;
	default:
		break;
	};

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Bind(unsigned int slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, glHandle);
}