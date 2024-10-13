#pragma once

#include "MathIncludes.h"

struct HexCameraData
{
	static vec2 offsetDirection;

	static float minZoom;
	static float maxZoom;
	static float zoomSpeed;
	static float startZoom;

	static float currentZoom;
};
