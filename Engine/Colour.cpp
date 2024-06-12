#include "Colour.h"

#include <cmath>

Colour::Colour() noexcept :
	r(1.0f),
	g(1.0f),
	b(1.0f),
	a(1.0f) {}
Colour::Colour(float rInit, float gInit, float bInit, float aInit) noexcept :
	r(rInit),
	g(gInit),
	b(bInit),
	a(aInit) {}

Colour Colour::HSVToRGB(Colour& hsv) noexcept
{
	Colour rgb;
	rgb.a = hsv.a;

	int h = (int)floor(hsv.h * 6);
	if (h == 6) h = 0;

	float chroma = hsv.v * hsv.s;
	float x = chroma * (1 - abs(fmod(hsv.h * 6, 2.0f) - 1));
	float m = hsv.v - chroma;

	switch (h)
	{
	case 0:
		rgb.r = chroma + m;
		rgb.g = x + m;
		rgb.b = 0 + m;
		break;
	case 1:
		rgb.r = x + m;
		rgb.g = chroma + m;
		rgb.b = 0 + m;
		break;
	case 2:
		rgb.r = 0 + m;
		rgb.g = chroma + m;
		rgb.b = x + m;
		break;
	case 3:
		rgb.r = 0 + m;
		rgb.g = x + m;
		rgb.b = chroma + m;
		break;
	case 4:
		rgb.r = x + m;
		rgb.g = 0 + m;
		rgb.b = chroma + m;
		break;
	case 5:
		rgb.r = chroma + m;
		rgb.g = 0 + m;
		rgb.b = x + m;
		break;
	}

	return rgb;
}
Colour Colour::RGBToHSV(Colour& rgb) noexcept
{
	Colour hsv;
	hsv.a = rgb.a;

	float max = rgb.r > rgb.g ? rgb.a : rgb.g;
	max = max > rgb.b ? max : rgb.b;
	float min = rgb.r < rgb.g ? rgb.a : rgb.g;
	min = min < rgb.b ? min : rgb.b;
	float delta = max - min;

	hsv.s = max == 0.0f ? 0.0f : delta / max;
	hsv.v = max;

	if (delta == 0.0f)
	{
		hsv.h = 0.0f;
	}
	else
	{
		if (max == rgb.r)
		{
			hsv.h = ((rgb.g - rgb.b) / delta) / 6;
		}
		else if (max == rgb.g)
		{
			hsv.h = ((rgb.b - rgb.r) / delta + 2) / 6;
		}
		else
		{
			hsv.h = ((rgb.r - rgb.g) / delta + 4) / 6;
		}
	}

	return hsv;
}

HDRColour::HDRColour(float rInit, float gInit, float bInit, float aInit, float iInit) noexcept :
	Colour(rInit, gInit, bInit, aInit),
	i(iInit) {}
HDRColour::HDRColour(Colour colourInit, float iInit) noexcept :
	Colour(colourInit),
	i(iInit) {}

HDRColour HDRColour::HSVToRGB(HDRColour& hsv) noexcept
{
	return HDRColour(Colour::HSVToRGB(hsv), hsv.i);
}
HDRColour HDRColour::RGBToHSV(HDRColour& rgb) noexcept
{
	return HDRColour(Colour::RGBToHSV(rgb), rgb.i);
}
