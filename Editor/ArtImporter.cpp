#include "ArtImporter.h"

#include "MeshRenderer.h"

#include "MeshHeirarchy.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "JsonIncludes.h"

#include "Vertex.h"

#include "Input.h"

#include "Debug.h"

namespace ArtImporter
{
	vector<string> fileQueue;
	uint current;
	MeshHeirarchy tempMeshes;
	MeshHeirarchy* selectedTempMesh;

	Assimp::Importer importer;
	const aiScene* currentFile;

	static void SetCurrent(uint newIndex)
	{
		if (newIndex == 0 || newIndex < fileQueue.size()) { current = newIndex; }
		else { current = (uint)fileQueue.size() - 1; }
		
		if (fileQueue.size() > 0) { Preload(fileQueue[current].c_str()); }
	}

	static void DrawNode(MeshHeirarchy* tempMesh)
	{
		bool isSelected = tempMesh == selectedTempMesh;
		bool isLeaf = tempMesh->children.size() == 0;

		int flags = 0;
		flags |= ImGuiTreeNodeFlags_DefaultOpen;
		flags |= ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
		flags |= ImGuiTreeNodeFlags_FramePadding;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		flags |= ImGuiTreeNodeFlags_Selected * isSelected;
		flags |= ImGuiTreeNodeFlags_Leaf * isLeaf;

		if (ImGui::TreeNodeEx(tempMesh->name.c_str(), flags))
		{
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenOverlapped))
			{
				if (!input->enabled && ImGui::IsKeyReleased(ImGuiKey_MouseLeft))
				{
					selectedTempMesh = tempMesh;
				}
			}
			for (int i = 0; i < tempMesh->children.size(); i++)
			{
				DrawNode(tempMesh->children[i]);
			}
			ImGui::TreePop();
		}
		else
		{
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenOverlapped))
			{
				if (!input->enabled && ImGui::IsKeyReleased(ImGuiKey_MouseLeft))
				{
					selectedTempMesh = tempMesh;
				}
			}
		}
	}

	void ArtImporter::Draw(const char* const name, bool& open) noexcept
	{
		ImGui::Begin(name, &open);

		string currentString = fileQueue.size() > 0 ? fileQueue[current] : "No files open";
		const uint relativeOffset = (uint)currentString.find("Assets\\");
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
			if (ImGui::Button(" < ")) { SetCurrent(current - 1); selectedTempMesh = nullptr; }
			ImGui::EndDisabled();
		}
		ImGui::SameLine();
		ImGui::Text((std::to_string(fileQueue.size() == 0 ? 0 : current + 1) + "/" + std::to_string(fileQueue.size())).c_str());
		ImGui::SameLine();
		{
			ImGui::BeginDisabled(fileQueue.size() == 0 || current >= fileQueue.size() - 1);
			if (ImGui::Button(" > ")) { SetCurrent(current + 1); selectedTempMesh = nullptr; }
			ImGui::EndDisabled();
		}

		if (ImGui::Button("Add Files"))
		{
			using namespace FileDialogue;
			vector<string> filepaths = GetLoadPaths(PathDetails("Open Mesh Files", "Assets\\", {"*.fbx", "*.obj", "*.gltf"}), LimitToAssetFolder::False);
			
			for (uint i = 0; i < filepaths.size(); i++)
			{
				Add(filepaths[i]);
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
				selectedTempMesh = nullptr;
			}
			ImGui::EndDisabled();
		}

		if (fileQueue.size() > 0)
		{
			if (current >= fileQueue.size()) SetCurrent((uint)fileQueue.size() - 1);

			if (ImGui::Button("Cook"))
			{
				CookCurrent();
				selectedTempMesh = nullptr;
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
				selectedTempMesh = nullptr;
			}

			ImGui::SeparatorText("Preview");
			DrawNode(&tempMeshes);

			if (selectedTempMesh != nullptr)
			{
				ImGui::SeparatorText("Mesh Settings");

				ImGui::InputText("Name", &selectedTempMesh->name);
				
				if (ImGui::BeginCombo("Material", selectedTempMesh->materialPath.c_str()))
				{
					if (ImGui::Selectable("Default", selectedTempMesh->materialPath == "Default"))
					{
						selectedTempMesh->materialPath = "Default";
					}
					if (ImGui::Selectable("Select from file"))
					{
						using namespace FileDialogue;
						string newFilepath = GetLoadPath(PathDetails("Select Material", "Assets\\Materials\\", { "*.mat" }), LimitToAssetFolder::True);

						if (newFilepath.size() != 0) selectedTempMesh->materialPath = newFilepath;
					}
					ImGui::EndCombo();
				}
			}
		}

		ImGui::End();
	}

	void Add(const string filepath) noexcept
	{
		ArtImporter::fileQueue.push_back(filepath);
		if (fileQueue.size() == 1) Preload(filepath.c_str());
	}

	static void ProcessMesh(const aiMesh* mesh, MeshHeirarchy& current) noexcept
	{
		current.name = mesh->mName.C_Str();
		current.filepath = "Engine\\Temp\\" + string(mesh->mName.C_Str()) + ".mesh";
		std::ofstream output(current.filepath, std::ios::binary);

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

	void ArtImporter::Preload(const char* filepath) noexcept
	{
		currentFile = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

		if (currentFile == nullptr)
		{
			std::ifstream filestream(filepath);
			if (filestream.good())
			{
				Debug::LogWarning(LogID::WRN102, "Found a Mesh file at filepath: ", filepath, ". But AssImp couldn't load it.", locationinfo);
			}
			else
			{
				Debug::LogWarning(LogID::WRN101, filepath, locationinfo);
			}
			return;
		}

		tempMeshes.Clear();
		ProcessNode(currentFile->mRootNode, tempMeshes);
	}

	static json CookHeirarchy(MeshHeirarchy* mesh)
	{
		string newPath;

		bool hasMesh = strcmp(mesh->name.c_str(), "GameObject3D") != 0;

		// Move Mesh
		if (hasMesh)
		{
			newPath = "Assets\\Meshes\\" + string(mesh->name) + ".mesh";
			std::rename(mesh->filepath.c_str(), newPath.c_str());
		}

		// Construct MeshRenderer for Prefab
		json meshRenderer;

		meshRenderer["Name"] = mesh->name;
		meshRenderer["GUID"] = GuidGenerator::NewGuid();
		meshRenderer["Active"] = true;
		meshRenderer["TypeID"] = hasMesh ? MeshRenderer::classID : GameObject3D::classID;
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
			meshRenderer["Material"] = mesh->materialPath;
		}

		vector<json> children;
		for (int i = 0; i < mesh->children.size(); i++)
		{
			children.push_back(CookHeirarchy(mesh->children[i]));
		}
		meshRenderer["Children"] = children;

		return meshRenderer;
	}

	void ArtImporter::CookCurrent() noexcept
	{
		json gameobject = CookHeirarchy(&tempMeshes);

		string filename = aiScene::GetShortFilename(fileQueue[current].c_str());
		uint extensionIndex = (uint)filename.find('.');
		if (extensionIndex != string::npos)
		{
			filename.erase(filename.begin() + extensionIndex, filename.end());
		}
		ofstream output("Assets\\Prefabs\\" + filename + ".prefab");
		output << std::setw(2) << gameobject;

		fileQueue.erase(fileQueue.begin() + current);
		if (current > 0 && current >= fileQueue.size()) SetCurrent(current - 1);
	}

	void ArtImporter::CookAll() noexcept
	{
		for (int i = 0; i < fileQueue.size(); i++)
		{
			CookCurrent();
		}
	}
}
