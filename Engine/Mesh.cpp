#include "Mesh.h"

#include "GeneralMacros.h"

#include <fstream>

#include "Debug.h"

Mesh::~Mesh()
{
	if (filepath != nullptr && strcmp(filepath, "None") != 0 && strcmp(filepath, "ProceduralQuad") != 0 && strcmp(filepath, "ProceduralCube") != 0)
	{
		delete filepath;
	}

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);
}

void GenerateTriTangBitang(Vertex& vert1, Vertex& vert2, Vertex& vert3)
{
	vec3 edgeA = vert2.position - vert1.position;
	vec3 edgeB = vert3.position - vert1.position;
	vec2 deltaTexCoordA = vert2.texCoord - vert1.texCoord;
	vec2 deltaTexCoordB = vert3.texCoord - vert1.texCoord;

	float fractionalPart = 1.0f / (deltaTexCoordA.x * deltaTexCoordB.y - deltaTexCoordB.x * deltaTexCoordA.y);
	
	vec4 newTang(0.0f);
	newTang.x = fractionalPart * (deltaTexCoordB.y * edgeA.x - deltaTexCoordA.y * edgeB.x);
	newTang.y = fractionalPart * (deltaTexCoordB.y * edgeA.y - deltaTexCoordA.y * edgeB.y);
	newTang.z = fractionalPart * (deltaTexCoordB.y * edgeA.z - deltaTexCoordA.y * edgeB.z);
	vert1.tangent = newTang; vert2.tangent = newTang; vert3.tangent = newTang;

	vec4 newBitang(0.0f);
	newBitang.x = fractionalPart * (-deltaTexCoordB.x * edgeA.x - -deltaTexCoordA.x * edgeB.x);
	newBitang.y = fractionalPart * (-deltaTexCoordB.x * edgeA.y - -deltaTexCoordA.x * edgeB.y);
	newBitang.z = fractionalPart * (-deltaTexCoordB.x * edgeA.z - -deltaTexCoordA.x * edgeB.z);
	vert1.biTangent = newBitang; vert2.biTangent = newBitang; vert3.biTangent = newBitang;
}
void GenerateQuadTangBitang(Vertex& vert1, Vertex& vert2, Vertex& vert3, Vertex& vert4)
{
	vec3 edgeA = vert2.position - vert1.position;
	vec3 edgeB = vert3.position - vert1.position;
	vec2 deltaTexCoordA = vert2.texCoord - vert1.texCoord;
	vec2 deltaTexCoordB = vert3.texCoord - vert1.texCoord;

	float fractionalPart = 1.0f / (deltaTexCoordA.x * deltaTexCoordB.y - deltaTexCoordB.x * deltaTexCoordA.y);

	vec4 newTang(0.0f);
	newTang.x = fractionalPart * (deltaTexCoordB.y * edgeA.x - deltaTexCoordA.y * edgeB.x);
	newTang.y = fractionalPart * (deltaTexCoordB.y * edgeA.y - deltaTexCoordA.y * edgeB.y);
	newTang.z = fractionalPart * (deltaTexCoordB.y * edgeA.z - deltaTexCoordA.y * edgeB.z);
	vert1.tangent = newTang; vert2.tangent = newTang; vert3.tangent = newTang; vert4.tangent = newTang;

	vec4 newBitang(0.0f);
	newBitang.x = fractionalPart * (-deltaTexCoordB.x * edgeA.x - -deltaTexCoordA.x * edgeB.x);
	newBitang.y = fractionalPart * (-deltaTexCoordB.x * edgeA.y - -deltaTexCoordA.x * edgeB.y);
	newBitang.z = fractionalPart * (-deltaTexCoordB.x * edgeA.z - -deltaTexCoordA.x * edgeB.z);
	vert1.biTangent = newBitang; vert2.biTangent = newBitang; vert3.biTangent = newBitang; vert4.biTangent = newBitang;
}

void Mesh::Initialise(uint vertexCount, const Vertex* vertices, uint indexCount, uint* indices)
{
	assert(vao == 0); // Check this is the first initialisation

	// Generate Buffers
	glGenBuffers(1, &vbo);
	glGenVertexArrays(1, &vao);

	// Bind Vertex Array
	glBindVertexArray(vao);

	// Bind Vertex Buffer
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	//Fill Vertex Buffer
	glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(Vertex), vertices, GL_STATIC_DRAW);

	// Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

	// Normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, normal)));

	// Tangent
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, tangent)));

	// BiTangent
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, biTangent)));

	// Texture Coords
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, texCoord)));

	// Bind Indices if there Are Any
	if (indexCount != 0)
	{
		glGenBuffers(1, &ibo);

		// Bind Vertex Buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

		// Fill Vertex Buffer
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(uint), indices, GL_STATIC_DRAW);

		triCount = indexCount / 3;
	}
	else
	{
		triCount = vertexCount / 3;
	}

	// Unbind Buffers
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void Mesh::InitialiseQuad()
{
	filepath = new char[15];
	const char* newFilepath = "ProceduralQuad";
	for (int i = 0; i < 15; i++) { filepath[i] = newFilepath[i]; }

	Vertex vertices[4]{};
	vertices[0].position = { -0.5f,  0,  0.5f, 1 };
	vertices[1].position = {  0.5f,  0,  0.5f, 1 };
	vertices[2].position = { -0.5f,  0, -0.5f, 1 };
	vertices[3].position = {  0.5f,  0, -0.5f, 1 };

	vertices[0].texCoord = { 0, 1 };
	vertices[1].texCoord = { 1, 1 };
	vertices[2].texCoord = { 0, 0 };
	vertices[3].texCoord = { 1, 0 };

	vertices[0].normal = { 0, 1, 0, 0 };
	vertices[1].normal = { 0, 1, 0, 0 };
	vertices[2].normal = { 0, 1, 0, 0 };
	vertices[3].normal = { 0, 1, 0, 0 };

	GenerateQuadTangBitang(vertices[0], vertices[1], vertices[2], vertices[3]);

	uint indices[6] =
	{
		0, 1, 2, 2, 1, 3
	};

	this->Initialise(4, vertices, 6, indices);
}
void Mesh::InitialiseCube()
{
	filepath = new char[15];
	const char* newFilepath = "ProceduralCube";
	for (int i = 0; i < 15; i++) { filepath[i] = newFilepath[i]; }

	Vertex vertices[24]{};

	for (int i = 0; i < 3; i++)
	{
		// Positions
		vertices[0 + 8 * i].position = { -0.5f, -0.5f,  0.5f, 1 };
		vertices[1 + 8 * i].position = { 0.5f, -0.5f,  0.5f, 1 };
		vertices[2 + 8 * i].position = { -0.5f, -0.5f, -0.5f, 1 };
		vertices[3 + 8 * i].position = { 0.5f, -0.5f, -0.5f, 1 };
		
		vertices[4 + 8 * i].position = { -0.5f,  0.5f,  0.5f, 1 };
		vertices[5 + 8 * i].position = { 0.5f,  0.5f,  0.5f, 1 };
		vertices[6 + 8 * i].position = { -0.5f,  0.5f, -0.5f, 1 };
		vertices[7 + 8 * i].position = { 0.5f,  0.5f, -0.5f, 1 };
	}

	// Texture Coordinates
	vertices[0].texCoord = { 0, 0 };
	vertices[1].texCoord = { 1, 0 };
	vertices[2].texCoord = { 0, 1 };
	vertices[3].texCoord = { 1, 1 };
	vertices[4].texCoord = { 0, 1 };
	vertices[5].texCoord = { 1, 1 };
	vertices[6].texCoord = { 0, 0 };
	vertices[7].texCoord = { 1, 0 };

	vertices[10].texCoord = { 1, 1 };
	vertices[11].texCoord = { 0, 1 };
	vertices[14].texCoord = { 1, 0 };
	vertices[15].texCoord = { 0, 0 };
	vertices[8].texCoord = { 0, 1 };
	vertices[9].texCoord = { 1, 1 };
	vertices[12].texCoord = { 0, 0 };
	vertices[13].texCoord = { 1, 0 };

	vertices[16].texCoord = { 1, 1 };
	vertices[18].texCoord = { 0, 1 };
	vertices[20].texCoord = { 1, 0 };
	vertices[22].texCoord = { 0, 0 };
	vertices[17].texCoord = { 0, 1 };
	vertices[19].texCoord = { 1, 1 };
	vertices[21].texCoord = { 0, 0 };
	vertices[23].texCoord = { 1, 0 };

	// Normals
	vec4 upNormal = { 0, 1, 0, 0 };
	vertices[0].normal  = -upNormal;
	vertices[1].normal  = -upNormal;
	vertices[2].normal  = -upNormal;
	vertices[3].normal  = -upNormal;
	vertices[4].normal  = upNormal;
	vertices[5].normal  = upNormal;
	vertices[6].normal  = upNormal;
	vertices[7].normal  = upNormal;

	vec4 forwardNormal = { 0, 0, 1, 0 };
	vertices[10].normal = -forwardNormal;
	vertices[11].normal = -forwardNormal;
	vertices[14].normal = -forwardNormal;
	vertices[15].normal = -forwardNormal;
	vertices[8].normal  = forwardNormal;
	vertices[9].normal  = forwardNormal;
	vertices[12].normal = forwardNormal;
	vertices[13].normal = forwardNormal;

	vec4 rightNormal = { 1, 0, 0, 0 };
	vertices[16].normal = -rightNormal;
	vertices[18].normal = -rightNormal;
	vertices[20].normal = -rightNormal;
	vertices[22].normal = -rightNormal;
	vertices[17].normal = rightNormal;
	vertices[19].normal = rightNormal;
	vertices[21].normal = rightNormal;
	vertices[23].normal = rightNormal;

	// Tangents + Bitangents
	GenerateQuadTangBitang(vertices[0], vertices[1], vertices[2], vertices[3]);
	GenerateQuadTangBitang(vertices[4], vertices[5], vertices[6], vertices[7]);

	GenerateQuadTangBitang(vertices[10], vertices[11], vertices[14], vertices[15]);
	GenerateQuadTangBitang(vertices[8], vertices[9], vertices[12], vertices[13]);

	GenerateQuadTangBitang(vertices[16], vertices[18], vertices[20], vertices[22]);
	GenerateQuadTangBitang(vertices[17], vertices[19], vertices[21], vertices[23]);

	uint indices[36] =
	{
		0,  2,  1,  1,  2,  3,
		4,  5,  6,  6,  5,  7,

		8,  9,  12, 12, 9,  13,
		11, 10, 15, 15, 10, 14,

		17, 19, 21, 21, 19, 23,
		18, 16, 22, 22, 16, 20,
	};

	this->Initialise(24, vertices, 36, indices);
}

bool Mesh::LoadFromFile(const char* filepathInit)
{
	assert(string(filepathInit).find(".mesh") != string::npos);

	std::ifstream filestream(filepathInit, std::ios::binary);

	if (!filestream.good())
	{
		Debug::Log(Debug::WRN( "Couldn't find a Mesh file at filepath: ", filepath, locationinfo ), Debug::WRN101);
		return false;
	}

	uint vertexCount;
	filestream.read((char*)&vertexCount, sizeof(uint));

	Vertex* vertices = new Vertex[vertexCount];
	filestream.read((char*)&vertices->position[0], vertexCount * sizeof(Vertex));

	uint indexCount;
	filestream.read((char*)&indexCount, sizeof(uint));

	uint* indices = new uint[indexCount];
	filestream.read((char*)&indices[0], indexCount * sizeof(uint));

	Initialise(vertexCount, vertices, indexCount, indices);
	
	delete[] vertices;
	delete[] indices;
	
	if (filepath != nullptr && strcmp(filepath, "None") != 0) { delete filepath; }

	uint filepathLength = (uint)strlen(filepathInit) + 1;
	filepath = new char[filepathLength];
	memcpy(filepath, filepathInit, filepathLength);

	return true;
}

const char* Mesh::GetFilePath() const
{
	return filepath;
}

void Mesh::Draw()
{
	glBindVertexArray(vao);

	// Using Indices or Just Vertices?
	if (ibo != 0)
	{
		glDrawElements(GL_TRIANGLES, 3 * triCount, GL_UNSIGNED_INT, 0);
	}
	else
	{
		glDrawArrays(GL_TRIANGLES, 0, 3 * triCount);
	}
}

void Mesh::DrawWireframe()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	Draw();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}