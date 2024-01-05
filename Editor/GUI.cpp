#include "GUI.h"

#include <sstream>

#include "GameObject.h"

std::string GUI::GenerateID(const char* name, GameObject* gameObject)
{
	std::ostringstream stream;
	stream << name << "##" << gameObject->GetGUID();

	return stream.str();
}