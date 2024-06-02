#include "MeshCooker.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <fstream>

#include "Vertex.h"

#include "Debug.h"

namespace MeshCooker
{
	vector<string> fileQueue;
	uint current;

	void MeshCooker::Draw(const char* const name, bool& open) noexcept
	{
		ImGui::Begin(name, &open);

		string currentString = fileQueue.size() > 0 ? fileQueue[current] : "No files open";
		const uint relativeOffset = currentString.find("Assets\\");
		if (relativeOffset != (uint)string::npos) currentString.erase(0, relativeOffset);

		{
			ImGui::BeginDisabled();
			ImGui::InputText("##CurrentFile", &currentString, ImGuiInputTextFlags_ReadOnly);
			ImGui::EndDisabled();
			ImGui::SetItemTooltip(currentString.c_str());
		}
		ImGui::SameLine();
		{
			ImGui::BeginDisabled(current == 0);
			if (ImGui::Button(" < ")) { current--; }
			ImGui::EndDisabled();
		}
		ImGui::SameLine();
		{
			ImGui::BeginDisabled(fileQueue.size() == 0 || current >= fileQueue.size() - 1);
			if (ImGui::Button(" > ")) { current++; }
			ImGui::EndDisabled();
		}

		if (ImGui::Button("Add Files"))
		{
			const char* const windowTitle = "Open Mesh Files";
			const uint defaultPathLength = 8;
			const char defaultPath[defaultPathLength] = "Assets\\";
			const uint filterPatternCount = 3;
			const char* const filterPatterns[filterPatternCount] = { "*.fbx", "*.obj", "*.gltf"};

			const char* const filePaths = tinyfd_openFileDialog(windowTitle, defaultPath, filterPatternCount, filterPatterns, nullptr, true);
			if (filePaths != nullptr)
			{
				const uint filePathLength = (uint)strlen(filePaths);

				string str;
				for (uint i = 0; i < filePathLength; i++)
				{
					if (filePaths[i] == '|')
					{
						fileQueue.push_back(str);
						str.clear();
						continue;
					}
					str.push_back(filePaths[i]);
				}
				if (str.size() > 0) fileQueue.push_back(str);
			}
		}
		
		{
			ImGui::BeginDisabled(fileQueue.size() == 0);
			ImGui::SameLine();
			if (ImGui::Button("Clear"))
			{
				current = 0;
				fileQueue.clear();
			}
			ImGui::EndDisabled();
		}

		if (fileQueue.size() > 0)
		{
			if (current >= fileQueue.size()) current = fileQueue.size() - 1;
			ImGui::Text((std::to_string(fileQueue.size()) + " files loaded").c_str());

			if (ImGui::Button("Cook"))
			{
				Cook(fileQueue[current].c_str());
				fileQueue.erase(fileQueue.begin() + current);
				if (current > 0 && current >= fileQueue.size()) current--;
			}
			ImGui::SameLine();
			if (ImGui::Button("Cook All"))
			{
				for (int i = 0; i < fileQueue.size(); i++)
				{
					Cook(fileQueue[i].c_str());
				}
				current = 0;
				fileQueue.clear();
			}
		}

		ImGui::End();
	}

	void MeshCooker::Cook(const char* filepath) noexcept
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

			uint vertexCount = (uint)mesh->mNumVertices;
			output.write((char*)&vertexCount, 1 * sizeof(uint));
			for (uint v = 0; v < mesh->mNumVertices; v++)
			{
				output.write((char*)&vec4(*(vec3*)&mesh->mVertices[v], 1)[0], 4 * sizeof(float));

				output.write((char*)&vec4(*(vec3*)&mesh->mNormals[v], 0)[0], 4 * sizeof(float));
				output.write((char*)&vec4(*(vec3*)&mesh->mTangents[v], 0)[0], 4 * sizeof(float));
				output.write((char*)&vec4(*(vec3*)&mesh->mBitangents[v], 0)[0], 4 * sizeof(float));

				output.write((char*)&mesh->mTextureCoords[0][v], 2 * sizeof(float));
			}

			uint faceCount = (uint)mesh->mNumFaces;
			uint indexCount = faceCount * 3;
			output.write((char*)&indexCount, 1 * sizeof(uint));
			for (uint f = 0; f < faceCount; f++)
			{
				uint indices[3] =
				{
					(uint)mesh->mFaces[f].mIndices[0],
					(uint)mesh->mFaces[f].mIndices[1],
					(uint)mesh->mFaces[f].mIndices[2]
				};
				output.write((char*)&indices[0], 3 * sizeof(uint));
			}

			output.close();
		}
	}
}
