#pragma once

#include <glm.hpp>
#include "glad.h"
#include "glfw3.h"

using glm::vec4;
using glm::vec2;

struct Vertex
{
	vec4 position;
	vec4 normal;
	vec2 texCoord;
};

class Mesh
{
public:
	Mesh() : triCount(0), vao(0), vbo(0), ibo(0) { }
	virtual ~Mesh();

	void Initialise(unsigned int vertexCount, const Vertex* vertices, unsigned int indexCount = 0, unsigned int* indices = nullptr);
	void InitialiseQuad();
	void InitialiseCube();

	virtual void Draw();
	virtual void DrawWireframe();

protected:
	unsigned int triCount;
	unsigned int vao, vbo, ibo;
};