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

	// Enable First Element as Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

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
		vertices[i].normal = { 0, 1, 0, 1 };
	}

	vertices[0].texCoord = { 0, 0 };
	vertices[1].texCoord = { 1, 0 };
	vertices[2].texCoord = { 0, 1 };
	vertices[3].texCoord = { 1, 1 };

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
		vertices[0 + 8 * i].position = { -0.5f, -0.5f,  0.5f, 1 };
		vertices[1 + 8 * i].position = { 0.5f, -0.5f,  0.5f, 1 };
		vertices[2 + 8 * i].position = { -0.5f, -0.5f, -0.5f, 1 };
		vertices[3 + 8 * i].position = { 0.5f, -0.5f, -0.5f, 1 };
		
		vertices[4 + 8 * i].position = { -0.5f,  0.5f,  0.5f, 1 };
		vertices[5 + 8 * i].position = { 0.5f,  0.5f,  0.5f, 1 };
		vertices[6 + 8 * i].position = { -0.5f,  0.5f, -0.5f, 1 };
		vertices[7 + 8 * i].position = { 0.5f,  0.5f, -0.5f, 1 };


		vec4 normal;
		if (i == 0) normal = { 0, 1, 0 , 1 };
		else if (i == 1) normal = { 0, 0, 1, 1 };
		else if (i == 2) normal = { 1, 0, 0, 1 };

		vertices[0 + 8 * i].normal = -normal;
		vertices[1 + 8 * i].normal = -normal;
		vertices[2 + 8 * i].normal = -normal;
		vertices[3 + 8 * i].normal = -normal;

		vertices[4 + 8 * i].normal = normal;
		vertices[5 + 8 * i].normal = normal;
		vertices[6 + 8 * i].normal = normal;
		vertices[7 + 8 * i].normal = normal;


		vertices[0 + 8 * i].texCoord = { 0, 0 };
		vertices[1 + 8 * i].texCoord = { 1, 0 };
		vertices[2 + 8 * i].texCoord = { 0, 1 };
		vertices[3 + 8 * i].texCoord = { 1, 1 };

		vertices[4 + 8 * i].texCoord = { 0, 0 };
		vertices[5 + 8 * i].texCoord = { 1, 0 };
		vertices[6 + 8 * i].texCoord = { 0, 1 };
		vertices[7 + 8 * i].texCoord = { 1, 1 };
	}

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