#pragma once

#include "GLIncludes.h"
#include "GeneralMacros.h"

#include "Vertex.h"

typedef unsigned int uint;

class Mesh
{
public:
	Mesh() : triCount(0), vao(0), vbo(0), ibo(0), filepath((char*)"None") { }
	virtual ~Mesh();
	uncopyable(Mesh)

	void Initialise(uint vertexCount, const Vertex* vertices, uint indexCount = 0, uint* indices = nullptr);
	void InitialiseQuad();
	void InitialiseCube();

	bool LoadFromFile(const char* filepath);

	const char* GetFilePath() const;

	virtual void Draw();
	virtual void DrawWireframe();

protected:
	unsigned int triCount;
	unsigned int vao, vbo, ibo;

	char* filepath;

	friend class MeshManager;
};