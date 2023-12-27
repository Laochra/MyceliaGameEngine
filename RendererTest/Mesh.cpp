#include "Mesh.h"

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);
}

void Mesh::Initialise(unsigned int vertexCount, const Vertex* vertices, unsigned int indexCount, unsigned int* indices)
{
	assert(vao == 0); // Check this is the first initialisation

	// Geberate Buffers
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

	// Texture Coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, texCoord)));

	// Bind Indices if there Are Any
	if (indexCount != 0)
	{
		glGenBuffers(1, &ibo);

		// Bind Vertex Buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

		// Fill Vertex Buffer
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);

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
	Vertex vertices[4];
	vertices[0].position = { -0.5f,  0,  0.5f, 1 };
	vertices[1].position = {  0.5f,  0,  0.5f, 1 };
	vertices[2].position = { -0.5f,  0, -0.5f, 1 };
	vertices[3].position = {  0.5f,  0, -0.5f, 1 };

	for (int i = 0; i < 4; i++)
	{
		vertices[i].normal = { 0, 1, 0, 0 };
	}

	vertices[0].texCoord = { 0, 1 };
	vertices[1].texCoord = { 1, 1 };
	vertices[2].texCoord = { 0, 0 };
	vertices[3].texCoord = { 1, 0 };

	unsigned int indices[6] =
	{
		0, 1, 2, 2, 1, 3
	};

	this->Initialise(4, vertices, 6, indices);
}

void Mesh::InitialiseCube()
{
	Vertex vertices[24];

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

	// Normals
	vec4 upNormal = { 0, 1, 0 , 0 };
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

	// Texture Coordinates
	vertices[0].texCoord  = { 0, 0 };
	vertices[1].texCoord  = { 1, 0 };
	vertices[2].texCoord  = { 0, 1 };
	vertices[3].texCoord  = { 1, 1 };
	vertices[4].texCoord  = { 0, 1 };
	vertices[5].texCoord  = { 1, 1 };
	vertices[6].texCoord  = { 0, 0 };
	vertices[7].texCoord  = { 1, 0 };

	vertices[10].texCoord = { 1, 1 };
	vertices[11].texCoord = { 0, 1 };
	vertices[14].texCoord = { 1, 0 };
	vertices[15].texCoord = { 0, 0 };
	vertices[8].texCoord  = { 0, 1 };
	vertices[9].texCoord  = { 1, 1 };
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

	unsigned int indices[36] =
	{
		0,  1,  2,  2,  1,  3,
		4,  5,  6,  6,  5,  7,

		8,  9,  12, 12, 9,  13,
		11, 10, 15, 15, 10, 14,

		17, 19, 21, 21, 19, 23,
		18, 16, 22, 22, 16, 20,
	};

	this->Initialise(24, vertices, 36, indices);
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