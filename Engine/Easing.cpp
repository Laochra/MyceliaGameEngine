#include "Easing.h"

#include <cmath>

/* Algorithms sourced from https://easings.net/ */

constexpr float pi = 3.14159265359;

static inline constexpr float sqr(float x) noexcept
{
	return x * x;
}
static inline constexpr float cube(float x) noexcept
{
	return x * x * x;
}
static inline constexpr float quart(float x) noexcept
{
	return x * x * x * x;
}
static inline constexpr float quint(float x) noexcept
{
	return x * x * x * x * x;
}
static inline float expo(float x, float e) noexcept
{
	return powf(x, e);
}

inline constexpr float Easing::QuadIn(float t) noexcept
{
	return sqr(t);
}
inline constexpr float Easing::QuadOut(float t) noexcept
{
	return 1.0f - sqr(1.0f - t);
}
inline constexpr float Easing::QuadInOut(float t) noexcept
{
	return t < 0.5f ?
		2.0f * sqr(t) :
		1.0f - sqr(-2.0f * t + 2.0f) * 0.5f;
}

inline constexpr float Easing::CubicIn(float t) noexcept
{
	return cube(t);
}
inline constexpr float Easing::CubicOut(float t) noexcept
{
	return 1.0f - cube(1.0f - t);
}
inline constexpr float Easing::CubicInOut(float t) noexcept
{
	return t < 0.5f ?
		4.0f * cube(t) :
		1.0f - cube(-2.0f * t + 2.0f) * 0.5f;
}

inline constexpr float Easing::QuartIn(float t) noexcept
{
	return quart(t);
}
inline constexpr float Easing::QuartOut(float t) noexcept
{
	return 1.0f - quart(1.0f - t);
}
inline constexpr float Easing::QuartInOut(float t) noexcept
{
	return t < 0.5f ?
		8.0f * quart(t) :
		1.0f - quart(-2.0f * t + 2.0f) * 0.5f;
}

inline constexpr float Easing::QuintIn(float t) noexcept
{
	return quint(t);
}
inline constexpr float Easing::QuintOut(float t) noexcept
{
	return 1.0f - quint(1.0f - t);
}
inline constexpr float Easing::QuintInOut(float t) noexcept
{
	return t < 0.5f ?
		16.0f * quint(t) :
		1.0f - quint(-2.0f * t + 2.0f) * 0.5f;
}

inline constexpr float Easing::BackIn(float t) noexcept
{
	constexpr float c1 = 1.70158f;
	constexpr float c3 = c1 + 1.0f;

	return c3 * cube(t) - c1 * sqr(t);
}
inline constexpr float Easing::BackOut(float t) noexcept
{
	constexpr float c1 = 1.70158f;
	constexpr float c3 = c1 + 1.0f;

	return 1.0f + c3 * cube(t - 1.0f) + c1 * sqr(t - 1.0f);
}
inline constexpr float Easing::BackInOut(float t) noexcept
{
	constexpr float c1 = 1.70158f;
	constexpr float c2 = c1 * 1.525f;

	return t < 0.5f ?
		(sqr(2.0f * t) * ((c2 + 1.0f) * 2.0f * t - c2)) * 0.5f :
		(sqr(2.0f * t - 2.0f) * ((c2 + 1.0f) * (t * 2.0f - 2.0f) + c2) + 2.0f) * 0.5f;
}

inline float Easing::SineIn(float t) noexcept
{
	return 1.0f - cos(t * pi * 0.5f);
}
inline float Easing::SineOut(float t) noexcept
{
	return sin(t * pi * 0.5f);
}
inline float Easing::SineInOut(float t) noexcept
{
	return -(cos(pi * t) - 1.0f) * 0.5f;
}

inline float Easing::ExpoIn(float t) noexcept
{
	return t == 0.0f ?
		0.0f :
		expo(2.0f, 10.0f * t - 10.0f);
}
inline float Easing::ExpoOut(float t) noexcept
{
	return t == 1.0f ?
		1.0f :
		1.0f - expo(2.0f, -10.0f * t);
}
inline float Easing::ExpoInOut(float t) noexcept
{
	return t == 0.0f ?
		0.0f :
		t == 1.0f ?
		1.0f :
		t < 0.5f ?
		expo(2.0f, 20.0f * t - 10.0f) * 0.5f :
		(2.0f - expo(2.0f, -20.0f * t + 10.0f)) * 0.5f;
}

inline float Easing::CircIn(float t) noexcept
{
	return 1.0f - sqrt(1.0f - sqr(t));
}
inline float Easing::CircOut(float t) noexcept
{
	return sqrt(1.0f - sqr(t - 1.0f));
}
inline float Easing::CircInOut(float t) noexcept
{
	return t < 0.5f ?
		(1.0f - sqrt(1.0f - sqr(2.0f * t))) * 0.5f :
		(sqrt(1.0f - sqr(-2.0f * t + 2.0f))) * 0.5f;
}
