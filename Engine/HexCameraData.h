#pragma once

#include "MathIncludes.h"

struct HexCameraData
{
	vec2 offsetDirection = glm::normalize(vec2(6.0f, 5.5f));

	float minZoom = 4.0f;
	float maxZoom = 12.0f;
	float zoomSpeed = 6.0f;
	float startZoom = 8.0f;

	float currentZoom = 8.0f;
};
