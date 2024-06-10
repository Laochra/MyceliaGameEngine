#pragma once

#include <vector>
#include "MathIncludes.h"

class MeshHeirarchy
{
public:
	const char* name;
	mat4 localMatrix;
	std::vector<MeshHeirarchy*> children;

	MeshHeirarchy(const char* nameInit = nullptr) noexcept;
	MeshHeirarchy(MeshHeirarchy& other) noexcept;
	MeshHeirarchy& operator=(MeshHeirarchy& other) noexcept;

	~MeshHeirarchy() noexcept;


	MeshHeirarchy& Add(const char* nameInit = nullptr) noexcept;
	void Clear() noexcept;
};
