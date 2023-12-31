#pragma once

#include <string>

class Texture
{
public:
	enum Format
	{
		None,
		SingleChannel,
		DualChannel,
		RGB,
		RGBA
	};

	Texture();
	Texture(const char* filename);
	Texture(unsigned int widthInit, unsigned int heightInit, Format formatInit, unsigned char* pixels = nullptr);
	virtual ~Texture();

	// Supports .jpg, .bmp, .png and .tga
	bool Load(const char* filename);
	void Create(unsigned int widthInit, unsigned int heightInit, Format formatInit, unsigned char* pixels = nullptr);

	// Returns the filename or "none" if not loaded from a file
	const std::string& GetFileName() const { return fileName; }

	// Binds the Texture to Specified Slot for GL Use
	void Bind(unsigned int slot) const;

	unsigned int GetGLHandle() const { return glHandle; }

	unsigned int GetWidth() const { return width; }
	unsigned int GetHeight() const { return height; }
	Format GetFormat() const { return format; }
	const unsigned char* GetPixels() const { return loadedPixels; }

protected:

	std::string	fileName;
	unsigned int width;
	unsigned int height;
	unsigned int glHandle;
	Format format;
	unsigned char* loadedPixels;
};