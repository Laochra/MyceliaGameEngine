#pragma once

struct Colour
{
	union { float r, red,	h, hue;			};
	union { float g, green, s, saturation; };
	union { float b, blue,	v, value;		};
	union { float a, alpha;						};

	Colour() noexcept;
	Colour(float rInit, float gInit, float bInit, float aInit = 1.0f) noexcept;

	static Colour HSVToRGB(Colour& hsv) noexcept;
	static Colour RGBToHSV(Colour& rgb) noexcept;
};
struct HDRColour : public Colour
{
	union { float i, intensity; };

	using Colour::Colour;
	HDRColour(float rInit, float gInit, float bInit, float aInit = 1.0f, float iInit = 1.0f) noexcept;
	HDRColour(Colour colourInit, float iInit = 1.0f) noexcept;

	static HDRColour HSVToRGB(HDRColour& hsv) noexcept;
	static HDRColour RGBToHSV(HDRColour& rgb) noexcept;
};
