#pragma once

#include <string>

class Texture
{
public:
	enum class Format
	{
		None,
		R,
		RG,
		RGB,
		RGBA
	};
	enum class Filter
	{
		None,
		Bilinear,
		Trilinear
	};

	Texture();
	Texture(const char* filename, bool isNonlinearInit, bool wrapInit, Filter filterInit);
	Texture(unsigned int widthInit, unsigned int heightInit, bool isNonlinearInit, bool wrapInit, Filter filterInit, Format formatInit, unsigned char* pixels = nullptr);
	virtual ~Texture();
	void Clear() noexcept;

	// Supports .jpg, .bmp, .png and .tga
	bool Load(const char* filename, bool isNonlinearInit, bool wrapInit, Filter filterInit);
	bool Reload();
	void Create(unsigned int widthInit, unsigned int heightInit, bool isNonlinearInit, bool wrapInit, Filter filterInit, Format formatInit, unsigned char* pixels = nullptr);

	// Returns the filepath or "None" if not loaded from a file
	const std::string& GetFilepath() const { return filepath; }

	// Binds the Texture to Specified Slot for GL Use
	void Bind(unsigned int slot) const;

	unsigned int GetGLHandle() const { return glHandle; }

	unsigned int GetWidth() const { return width; }
	unsigned int GetHeight() const { return height; }

	Format GetFormat() const { return format; }
	Filter GetFilter() const { return filter; }
	bool GetIsNonlinear() const { return isNonlinear; }
	bool GetWrap() const { return wrap; }

protected:
	std::string	filepath;
	unsigned int glHandle;

	unsigned int width;
	unsigned int height;

	Format format = Format::RGB;
	Filter filter = Filter::None;
	bool isNonlinear = false;
	bool wrap = false;

	friend class TextureManager;
	friend class Material;
};