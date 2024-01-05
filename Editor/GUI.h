#pragma once

#include <string>

class GameObject;

namespace GUI
{
	extern std::string GenerateID(const char* name, GameObject* gameObject);
}