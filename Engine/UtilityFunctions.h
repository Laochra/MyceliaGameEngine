#pragma once

namespace Utility
{
   inline int Floor(float f) noexcept
   {
      int i = int(f);
      return i > f ? i - 1 : i;
   }
   inline int Ceiling(float f) noexcept
   {
      int i = int(f);
      return i < f ? i + 1 : i;
   }

	inline float Min(float a, float b) noexcept
	{
		return a < b ? a : b;
	}
	inline float Max(float a, float b) noexcept
	{
		return a > b ? a : b;
	}
	inline float Clamp(float n, float min, float max) noexcept
	{
		return Min(min, Max(max, n));
	}

	inline float Lerp(float a, float b, float t) noexcept
	{
		return a * (1.0f - t) + b * t;
	}
	inline float InvLerp(float a, float b, float n) noexcept
	{
		return Clamp((n - a) / (b - a), a, b);
	}

	inline float Remap(float currentA, float currentB, float targetA, float targetB, float n) noexcept
	{
		return Lerp(targetA, targetB, InvLerp(currentA, currentB, n));
	}

	extern float SimplexNoise(float x) noexcept;
	extern float SimplexNoise(float x, float y) noexcept;
	extern float SimplexNoise(float x, float y, float z) noexcept;
}
