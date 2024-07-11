#pragma once

#include "GLIncludes.h"
#include "GeneralMacros.h"

#include "Vertex.h"

typedef unsigned int uint;

class Mesh
{
public:
	Mesh() : primitiveCount(0), vao(0), vbo(0), ibo(0), filepath((char*)"None") { }
	virtual ~Mesh();
	uncopyable(Mesh)

	void Initialise(uint vertexCount, const Vertex* vertices, uint indexCount = 0, uint* indices = nullptr);
	void InitialiseQuad();
	void InitialiseCube();
	void InitialiseCone(); // TODO: Fix the normals / tangents / bitangents for using with lit shaders
	void InitialiseRing(uint ringSides = 32);

	bool LoadFromFile(const char* filepath);

	const char* GetFilePath() const;

	virtual void Draw();
	virtual void DrawWireframe();

protected:
	unsigned int primitiveCount;
	unsigned int primitiveType = GL_TRIANGLES;
	unsigned int pointsPerPrimitive = 3;
	unsigned int vao, vbo, ibo;

	char* filepath;

	friend class MeshManager;
};