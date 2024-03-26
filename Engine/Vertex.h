#pragma once

#include "GLIncludes.h"

struct Vertex
{
	vec4 position;

	vec4 normal;
	vec4 tangent;
	vec4 biTangent;

	vec2 texCoord;
};