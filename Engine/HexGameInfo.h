#pragma once

#include "MathIncludes.h"

#include "JsonIncludes.h"
#include "GameObject3D.h"

struct HexGameInfo
{
	static vec4 highlightColour;

	static void LoadBackgroundPrefab(const string& filepath);
	static std::string backgroundFilepath;
	static json backgroundPrefab;
	static GameObject3D* background;
};
