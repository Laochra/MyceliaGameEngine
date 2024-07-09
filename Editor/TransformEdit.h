#pragma once

#include "MathIncludes.h"

namespace TransformEdit
{
	enum class Space : bool
	{
		Local,
		Global
	};
	enum class Mode
	{
		Select,
		Translate,
		Rotate
	};
	enum class Handle : unsigned long long
	{
		None,

		X,
		Y,
		Z
	};

	extern Space space;
	extern Mode mode;

	extern void Update() noexcept;

	extern void DrawIDs() noexcept;
	extern void Draw() noexcept;

	extern void BeginTransform(Handle handle, vec2 normalisedMousePos) noexcept;
}
