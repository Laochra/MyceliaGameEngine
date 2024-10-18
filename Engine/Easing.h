#pragma once

/* Algorithms sourced from https://easings.net/ */

namespace Easing
{
	extern constexpr float QuadIn(float t) noexcept;
	extern constexpr float QuadOut(float t) noexcept;
	extern constexpr float QuadInOut(float t) noexcept;

	extern constexpr float CubicIn(float t) noexcept;
	extern constexpr float CubicOut(float t) noexcept;
	extern constexpr float CubicInOut(float t) noexcept;

	extern constexpr float QuartIn(float t) noexcept;
	extern constexpr float QuartOut(float t) noexcept;
	extern constexpr float QuartInOut(float t) noexcept;

	extern constexpr float QuintIn(float t) noexcept;
	extern constexpr float QuintOut(float t) noexcept;
	extern constexpr float QuintInOut(float t) noexcept;

	extern constexpr float BackIn(float t) noexcept;
	extern constexpr float BackOut(float t) noexcept;
	extern constexpr float BackInOut(float t) noexcept;


	// Expensive! Uses cos(), use Easing::QuadIn() for similar results
	extern float SineIn(float t) noexcept;
	// Expensive! Uses sin(), use Easing::QuadOut() for similar results
	extern float SineOut(float t) noexcept;
	// Expensive! Uses cos(), use Easing::QuadInOut() for similar results
	extern float SineInOut(float t) noexcept;

	// Expensive! Uses pow(), use Easing::QuintIn() for similar results
	extern float ExpoIn(float t) noexcept;
	// Expensive! Uses pow(), use Easing::QuintOut() for similar results
	extern float ExpoOut(float t) noexcept;
	// Expensive! Uses pow(), use Easing::QuintInOut() for similar results
	extern float ExpoInOut(float t) noexcept;

	// Expensive! Uses sqrt()
	extern float CircIn(float t) noexcept;
	// Expensive! Uses sqrt()
	extern float CircOut(float t) noexcept;
	// Expensive! Uses sqrt(), use Easing::QuintInOut() for similar results
	extern float CircInOut(float t) noexcept;
}
