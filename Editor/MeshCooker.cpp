#include "MeshCooker.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <fstream>
#include "json.hpp"
using nlohmann::json;

#include "Vertex.h"

#include "Debug.h"

static void Read(const char* filepath) // Test - Working!
{
	std::ifstream filestream(filepath, std::ios::binary);

	uint vertexCount;
	filestream.read((char*)&vertexCount, sizeof(uint));

	Vertex* vertices = new Vertex[vertexCount];
	filestream.read((char*)&vertices->position[0], vertexCount * sizeof(Vertex));

	uint indexCount;
	filestream.read((char*)&indexCount, sizeof(uint));

	uint* indices = new uint[indexCount];
	filestream.read((char*)&indices[0], indexCount * sizeof(uint));
}

void MeshCooker::Cook(const char* filepath)
{
	Assimp::Importer importer;
	const aiScene* file = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	if (file == nullptr)
	{
		std::ifstream filestream(filepath);
		if (filestream.good())
		{
			debug->Log({ "Found a Mesh file at filepath: ", filepath, ". But AssImp couldn't load it." locationinfo }, Debug::Warning, Debug::WRN102);
		}
		else
		{
			debug->Log({ "Couldn't find a Mesh file at filepath: ", filepath, locationinfo }, Debug::Warning, Debug::WRN101);
		}
	}

	for (int m = 0; m < file->mNumMeshes; m++)
	{
		aiMesh* mesh = file->mMeshes[m];

		std::ofstream output("Assets\\Meshes\\" + string(mesh->mName.C_Str()) + ".mesh", std::ios::binary);

		output.write((char*)&mesh->mNumVertices, 1 * sizeof(uint));
		for (uint v = 0; v < mesh->mNumVertices; v++)
		{
			output.write((char*)&vec4(*(vec3*)&mesh->mVertices[v], 1)[0], 4 * sizeof(float));

			output.write((char*)&vec4(*(vec3*)&mesh->mNormals[v], 0)[0], 4 * sizeof(float));
			output.write((char*)&vec4(*(vec3*)&mesh->mTangents[v], 0)[0], 4 * sizeof(float));
			output.write((char*)&vec4(*(vec3*)&mesh->mBitangents[v], 0)[0], 4 * sizeof(float));
			
			output.write((char*)&mesh->mTextureCoords[0][v], 2 * sizeof(float));
		}
		
		uint indexCount = mesh->mNumFaces * 3;
		output.write((char*)&indexCount, 1 * sizeof(uint));
		for (uint f = 0; f < mesh->mNumFaces; f++)
		{
			output.write((char*)&mesh->mFaces[f].mIndices[0], 3 * sizeof(uint));
		}

		output.close();
		Read(("Assets\\Meshes\\" + string(mesh->mName.C_Str()) + ".mesh").c_str());
	}
}
