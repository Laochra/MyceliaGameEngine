#pragma once

#include <vector>
#include <string>
#include "MathIncludes.h"

class MeshHeirarchy
{
public:
	std::string name;
	std::string filepath = "None";
	mat4 localMatrix;
	std::string materialPath;
	std::vector<MeshHeirarchy*> children;

	MeshHeirarchy(const char* filepathInit = "None", const char* nameInit = "None", const char* materialPathInit = "Default") noexcept;
	MeshHeirarchy(MeshHeirarchy& other) noexcept;
	MeshHeirarchy& operator=(MeshHeirarchy& other) noexcept;

	~MeshHeirarchy() noexcept;


	MeshHeirarchy& Add(const char* filepathInit = "None", const char* nameInit = "None") noexcept;
	void Clear() noexcept;
};
