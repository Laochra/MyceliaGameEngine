#include "MeshCooker.h"

#include "MeshRenderer.h"

#include "MeshHeirarchy.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "JsonIncludes.h"

#include "Vertex.h"

#include "Debug.h"

namespace MeshCooker
{
	vector<string> fileQueue;
	uint current;
	MeshHeirarchy tempMeshes;

	Assimp::Importer importer;
	const aiScene* currentFile;

	static void SetCurrent(uint newIndex)
	{
		if (newIndex == 0 || newIndex < fileQueue.size())
		{
			current = newIndex;
		}
		else
		{
			current = fileQueue.size() - 1;
		}
		
		if (fileQueue.size() > 0)
		{
			Preload(fileQueue[current].c_str());
		}
	}

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
			if (ImGui::Button(" < ")) { SetCurrent(current - 1); }
			ImGui::EndDisabled();
		}
		ImGui::SameLine();
		ImGui::Text((std::to_string(fileQueue.size() == 0 ? 0 : current + 1) + "/" + std::to_string(fileQueue.size())).c_str());
		ImGui::SameLine();
		{
			ImGui::BeginDisabled(fileQueue.size() == 0 || current >= fileQueue.size() - 1);
			if (ImGui::Button(" > ")) { SetCurrent(current + 1); }
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
						Add(str);
						str.clear();
						continue;
					}
					str.push_back(filePaths[i]);
				}
				if (str.size() > 0) Add(str);
			}
		}
		
		{
			ImGui::BeginDisabled(fileQueue.size() == 0);
			ImGui::SameLine();
			if (ImGui::Button("Clear"))
			{
				fileQueue.clear();
				tempMeshes.Clear();
				SetCurrent(0);
			}
			ImGui::EndDisabled();
		}

		if (fileQueue.size() > 0)
		{
			if (current >= fileQueue.size()) SetCurrent(fileQueue.size() - 1);

			if (ImGui::Button("Cook"))
			{
				CookCurrent();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cook All"))
			{
				for (int i = 0; i < fileQueue.size(); i++)
				{
					CookAll();
				}
				fileQueue.clear();
				tempMeshes.Clear();
				SetCurrent(0);
			}
		}

		ImGui::End();
	}

	void Add(const string filepath) noexcept
	{
		MeshCooker::fileQueue.push_back(filepath);
		if (fileQueue.size() == 1) Preload(filepath.c_str());
	}

	static void ProcessMesh(const aiMesh* mesh, MeshHeirarchy& current) noexcept
	{
		current.name = mesh->mName.C_Str();
		std::ofstream output("Engine\\Temp\\" + string(mesh->mName.C_Str()) + ".mesh", std::ios::binary);

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

	static void ProcessNode(aiNode* node, MeshHeirarchy& current, mat4 initialMatrix = glm::identity<mat4>()) noexcept
	{
		bool hasMesh = node->mNumMeshes != 0;

		current.localMatrix = initialMatrix * glm::transpose(*(mat4*)&node->mTransformation);

		if (hasMesh)
		{
			ProcessMesh(currentFile->mMeshes[node->mMeshes[0]], current);
		}
		else
		{
			current.name = "GameObject3D";
			if (node->mNumChildren == 1)
			{
				ProcessNode(node->mChildren[0], current, current.localMatrix);
				return;
			}
		}

		for (uint i = 0; i < node->mNumChildren; i++)
		{
			ProcessNode(node->mChildren[i], current.Add());
		}
	}

	void MeshCooker::Preload(const char* filepath) noexcept
	{
		currentFile = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

		if (currentFile == nullptr)
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
			return;
		}

		tempMeshes.Clear();
		ProcessNode(currentFile->mRootNode, tempMeshes);
	}

	static json CookHeirarchy(MeshHeirarchy* mesh)
	{
		string newPath;

		bool hasMesh = strcmp(mesh->name, "GameObject3D") != 0;

		// Move Mesh
		if (hasMesh)
		{
			string oldPath = "Engine\\Temp\\" + string(mesh->name) + ".mesh";
			newPath = "Assets\\Meshes\\" + string(mesh->name) + ".mesh";
			std::rename(oldPath.c_str(), newPath.c_str());
		}

		// Construct MeshRenderer for Prefab
		json meshRenderer;

		meshRenderer["Name"] = mesh->name;
		meshRenderer["GUID"] = GuidGenerator::NewGuid();
		meshRenderer["Active"] = true;
		meshRenderer["TypeID"] = hasMesh ? MeshRenderer::classID : GameObject3D::classID;
		// TODO: Actually get transforms and pass those through,
		// make sure to apply parent transforms to children if a parent is culled for being useless
		const mat4& t = mesh->localMatrix;
		const vec3 position(t[3]);
		meshRenderer["Position"] = vector{ position.x, position.y, position.z };
		const vec3 scale(glm::length((vec3)t[0]), glm::length((vec3)t[1]), glm::length((vec3)t[2]));
		meshRenderer["Scale"] = vector{ scale.x, scale.y, scale.z };
		const mat3 rotMat((vec3)t[0] / scale[0], (vec3)t[1] / scale[1], (vec3)t[2] / scale[2]);
		const quat rotation = glm::quat_cast(rotMat);
		meshRenderer["Rotation"] = vector{ rotation.w, rotation.x, rotation.y, rotation.z };
		meshRenderer["Pivot"] = vector{ 0.0f, 0.0f, 0.0f };
		if (hasMesh)
		{
			meshRenderer["Mesh"] = newPath;
			meshRenderer["Material"] = "Default";
		}

		vector<json> children;
		for (int i = 0; i < mesh->children.size(); i++)
		{
			children.push_back(CookHeirarchy(mesh->children[i]));
		}
		meshRenderer["Children"] = children;

		return meshRenderer;
	}

	void MeshCooker::CookCurrent() noexcept
	{
		json gameobject = CookHeirarchy(&tempMeshes);

		string filename = aiScene::GetShortFilename(fileQueue[current].c_str());
		uint extensionIndex = filename.find('.');
		if (extensionIndex != string::npos)
		{
			filename.erase(filename.begin() + extensionIndex, filename.end());
		}
		ofstream output("Assets\\Prefabs\\" + filename + ".prefab");
		output << std::setw(2) << gameobject;

		fileQueue.erase(fileQueue.begin() + current);
		if (current > 0 && current >= fileQueue.size()) SetCurrent(current - 1);
	}

	void MeshCooker::CookAll() noexcept
	{
		for (int i = 0; i < fileQueue.size(); i++)
		{
			CookCurrent();
		}
	}
}
