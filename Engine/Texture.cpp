#include "glad.h"
#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Debug.h"

Texture::Texture() : filepath("None"), width(0), height(0), glHandle(0), format(Format::None) { }
Texture::Texture(const char* filepathInit, bool isNonlinear, bool wrap, Filter filter) : filepath("None"), width(0), height(0), glHandle(0), format(Format::None)
{
	Load(filepathInit, isNonlinear, wrap, filter);
}
Texture::Texture(unsigned int widthInit, unsigned int heightInit, bool isNonlinear, bool wrap, Filter filter, Format formatInit, unsigned char* pixels) :
	filepath("None"), width(widthInit), height(heightInit), format(formatInit)
{
	Create(width, height, isNonlinear, wrap, filter, formatInit, pixels);
}

Texture::~Texture()
{
	if (glHandle != 0) glDeleteTextures(1, &glHandle);
}

void Texture::Clear() noexcept
{
	if (glHandle != 0) glDeleteTextures(1, &glHandle);;
}

bool Texture::Load(const char* filepathInit, bool isNonlinearInit, bool wrapInit, Filter filterInit)
{
	if (glHandle != 0)
	{
		glDeleteTextures(1, &glHandle);
		glHandle = 0;
		width = 0;
		height = 0;
		filepath = "None";
	}

	int x = 0, y = 0, comp = 0;
	unsigned char* loadedPixels = stbi_load(filepathInit, &x, &y, &comp, STBI_default);

	if (loadedPixels != nullptr)
	{
		filter = filterInit;
		isNonlinear = isNonlinearInit;
		wrap = wrapInit;

		glGenTextures(1, &glHandle);
		glBindTexture(GL_TEXTURE_2D, glHandle);
		switch (comp)
		{
		case STBI_grey:
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, x, y, 0, GL_RED, GL_UNSIGNED_BYTE, loadedPixels);
			format = Format::R;
			break;
		}
		case STBI_grey_alpha:
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, x, y, 0, GL_RG, GL_UNSIGNED_BYTE, loadedPixels);
			format = Format::RG;
			break;
		}
		case STBI_rgb:
		{
			glTexImage2D(GL_TEXTURE_2D, 0, isNonlinear ? GL_SRGB : GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, loadedPixels);
			format = Format::RGB;
			break;
		}
		case STBI_rgb_alpha:
		{
			glTexImage2D(GL_TEXTURE_2D, 0, isNonlinear ? GL_SRGB_ALPHA : GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, loadedPixels);
			format = Format::RGBA;
			break;
		}
		default: break;
		};

		switch (filter)
		{
		case Filter::None:
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			break;
		}
		case Filter::Bilinear:
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			break;
		}
		case Filter::Trilinear:
		{
			glGenerateMipmap(GL_TEXTURE_2D);
			float anisotropy = 0.0f;
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &anisotropy);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, anisotropy);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			break;
		}
		}
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap ? GL_REPEAT : GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap ? GL_REPEAT : GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);
		width = (unsigned int)x;
		height = (unsigned int)y;
		filepath = filepathInit;
		stbi_image_free(loadedPixels);
		return true;
	}
	return false;
}

bool Texture::Reload()
{
	return Load(filepath.c_str(), isNonlinear, wrap, filter);
}

void Texture::Create(unsigned int widthInit, unsigned int heightInit, bool isNonlinearInit, bool wrapInit, Filter filterInit, Format formatInit, unsigned char* pixels)
{
	if (glHandle != 0) {
		glDeleteTextures(1, &glHandle);
		glHandle = 0;
		filepath = "None";
	}

	width = widthInit;
	height = heightInit;
	format = formatInit;
	filter = filterInit;
	isNonlinear = isNonlinearInit;
	wrap = wrapInit;

	glGenTextures(1, &glHandle);
	glBindTexture(GL_TEXTURE_2D, glHandle);

	switch (format)
	{
	case Format::R: glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, pixels);
		break;
	case Format::RG: glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, width, height, 0, GL_RG, GL_UNSIGNED_BYTE, pixels);
		break;
	case Format::RGB: glTexImage2D(GL_TEXTURE_2D, 0, isNonlinear ? GL_SRGB : GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
		break;
	case Format::RGBA: glTexImage2D(GL_TEXTURE_2D, 0, isNonlinear ? GL_SRGB_ALPHA : GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
		break;
	default:
		break;
	};

	switch (filter)
	{
	case Filter::None:
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		break;
	}
	case Filter::Bilinear:
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		break;
	}
	case Filter::Trilinear:
	{
		glGenerateMipmap(GL_TEXTURE_2D);
		float anisotropy = 0.0f;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &anisotropy);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, anisotropy);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		break;
	}
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap ? GL_REPEAT : GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap ? GL_REPEAT : GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Bind(unsigned int slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, glHandle);
}