#pragma once

#include "MathIncludes.h"
#include "JsonIncludes.h"

#include <string>
using std::string;

class GameObject3D;
class LightObject;

struct HexGameInfo
{
	static vec4 highlightColour;

	static void LoadBackgroundPrefab(const string& filepath);
	static std::string backgroundFilepath;
	static json backgroundPrefab;
	static GameObject3D* background;

	static void LoadLightingPrefab(const string& filepath);
	static std::string lightingFilepath;
	static json lightingPrefab;
	static LightObject* lighting;
};
