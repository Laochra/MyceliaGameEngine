#pragma once

/* Algorithms sourced from https://easings.net/ */

namespace Easing
{
	inline constexpr float QuadIn(float t) noexcept;
	inline constexpr float QuadOut(float t) noexcept;
	inline constexpr float QuadInOut(float t) noexcept;

	inline constexpr float CubicIn(float t) noexcept;
	inline constexpr float CubicOut(float t) noexcept;
	inline constexpr float CubicInOut(float t) noexcept;

	inline constexpr float QuartIn(float t) noexcept;
	inline constexpr float QuartOut(float t) noexcept;
	inline constexpr float QuartInOut(float t) noexcept;

	inline constexpr float QuintIn(float t) noexcept;
	inline constexpr float QuintOut(float t) noexcept;
	inline constexpr float QuintInOut(float t) noexcept;

	inline constexpr float BackIn(float t) noexcept;
	inline constexpr float BackOut(float t) noexcept;
	inline constexpr float BackInOut(float t) noexcept;


	// Expensive! Uses cos(), use Easing::QuadIn() for similar results
	inline float SineIn(float t) noexcept;
	// Expensive! Uses sin(), use Easing::QuadOut() for similar results
	inline float SineOut(float t) noexcept;
	// Expensive! Uses cos(), use Easing::QuadInOut() for similar results
	inline float SineInOut(float t) noexcept;

	// Expensive! Uses pow(), use Easing::QuintIn() for similar results
	inline float ExpoIn(float t) noexcept;
	// Expensive! Uses pow(), use Easing::QuintOut() for similar results
	inline float ExpoOut(float t) noexcept;
	// Expensive! Uses pow(), use Easing::QuintInOut() for similar results
	inline float ExpoInOut(float t) noexcept;

	// Expensive! Uses sqrt()
	inline float CircIn(float t) noexcept;
	// Expensive! Uses sqrt()
	inline float CircOut(float t) noexcept;
	// Expensive! Uses sqrt(), use Easing::QuintInOut() for similar results
	inline float CircInOut(float t) noexcept;
}
