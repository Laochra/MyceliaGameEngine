#pragma once

#include "GLIncludes.h"

#include "Vertex.h"

class Mesh
{
public:
	Mesh() : triCount(0), vao(0), vbo(0), ibo(0), filepath((char*)"None") { }
	virtual ~Mesh();

	void Initialise(unsigned int vertexCount, const Vertex* vertices, unsigned int indexCount = 0, unsigned int* indices = nullptr);
	void InitialiseQuad();
	void InitialiseCube();

	const char* GetFilePath() const;

	virtual void Draw();
	virtual void DrawWireframe();

protected:
	unsigned int triCount;
	unsigned int vao, vbo, ibo;

	char* filepath;
};