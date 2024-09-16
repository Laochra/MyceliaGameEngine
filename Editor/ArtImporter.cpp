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

#include "AppInfo.h"

namespace ArtImporter
{
	vector<string> fileQueue;
	uint current;

	bool compressHeirarchy = true;
	bool applyTransforms = false;

	MeshHeirarchy tempMeshes;
	MeshHeirarchy* selectedTempMesh;

	Assimp::Importer importer;
	const aiScene* currentFile;

	static void SetCurrent(uint newIndex)
	{
		if (newIndex < fileQueue.size()) { current = newIndex; }
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

		if (ImGui::TreeNodeEx(StringBuilder(tempMesh->name, "##Mesh").CStr(), flags))
		{
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenOverlapped))
			{
				if (!AppInfo::input->enabled && ImGui::IsKeyReleased(ImGuiKey_MouseLeft))
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
				if (!AppInfo::input->enabled && ImGui::IsKeyReleased(ImGuiKey_MouseLeft))
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

		if (ImGui::Checkbox("Compress Heirarchy", &compressHeirarchy))
		{
			if (fileQueue.size() > 0) Preload(fileQueue[current].c_str());
		}
		if (ImGui::Checkbox("Respect Transform Data", &applyTransforms))
		{
			if (fileQueue.size() > 0) Preload(fileQueue[current].c_str());
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
				using namespace FileDialogue;
				PathDetails savePathDetails(
					"Cook Prefab",
					"Assets\\NewPrefab.prefab",
					{ "*.prefab" }
				);
				string savePath = GetSavePath(savePathDetails, LimitToAssetFolder::True);
				if (savePath.size() != 0)
				{
					Cook(current, savePath);
				}
				selectedTempMesh = nullptr;
			}
			ImGui::SameLine();
			if (ImGui::Button("Cook All"))
			{
				using namespace FileDialogue;
				PathDetails savePathDetails(
					"Choose Target Location",
					"Assets\\"
				);
				string savePath = GetFolderPath(savePathDetails, LimitToAssetFolder::True);
				
				if (savePath.size() != 0)
				{
					CookAll(savePath);

					fileQueue.clear();
					tempMeshes.Clear();
					SetCurrent(0);
					selectedTempMesh = nullptr;
				}
			}

			ImGui::SeparatorText("Preview");
			DrawNode(&tempMeshes);

			if (selectedTempMesh != nullptr)
			{
				ImGui::SeparatorText("Mesh Settings");

				if (ImGui::InputText("Name", &selectedTempMesh->name))
				{
					if (selectedTempMesh->name.size() == 0) selectedTempMesh->name = "Empty";
				}
				
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

		if (applyTransforms)
		{
			current.localMatrix = initialMatrix * glm::transpose(*(mat4*)&node->mTransformation);
		}
		else
		{
			current.localMatrix = glm::identity<mat4>();
		}

		if (hasMesh)
		{
			ProcessMesh(currentFile->mMeshes[node->mMeshes[0]], current);
		}
		else
		{
			current.name = "Empty";

			if (compressHeirarchy && node->mNumChildren == 1)
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

	static json CookHeirarchy(MeshHeirarchy* meshNode, string saveLocation)
	{
		string newPath;

		bool hasMesh = meshNode->filepath != "None";

		// Move Mesh
		if (hasMesh)
		{
			newPath = saveLocation + string(meshNode->name) + ".mesh";
			std::remove(newPath.c_str());
			std::rename(meshNode->filepath.c_str(), newPath.c_str());
		}

		// Construct GameObject for Prefab
		json gameObject;

		gameObject["Name"] = meshNode->name;
		gameObject["GUID"] = GuidGenerator::NewGuid();
		gameObject["Active"] = true;
		gameObject["TypeID"] = hasMesh ? MeshRenderer::classID : GameObject3D::classID;
		const mat4& t = meshNode->localMatrix;
		const vec3 position(t[3]);
		gameObject["Position"] = vector{ position.x, position.y, position.z };
		const vec3 scale(glm::length((vec3)t[0]), glm::length((vec3)t[1]), glm::length((vec3)t[2]));
		gameObject["Scale"] = vector{ scale.x, scale.y, scale.z };
		const mat3 rotMat((vec3)t[0] / scale[0], (vec3)t[1] / scale[1], (vec3)t[2] / scale[2]);
		const quat rotation = glm::quat_cast(rotMat);
		gameObject["Rotation"] = vector{ rotation.w, rotation.x, rotation.y, rotation.z };
		gameObject["Pivot"] = vector{ 0.0f, 0.0f, 0.0f };
		if (hasMesh)
		{
			gameObject["Mesh"] = newPath;
			gameObject["Material"] = meshNode->materialPath;
		}

		vector<json> children;
		for (int i = 0; i < meshNode->children.size(); i++)
		{
			children.push_back(CookHeirarchy(meshNode->children[i], saveLocation));
		}
		gameObject["Children"] = children;

		return gameObject;
	}

	void ArtImporter::Cook(uint index, string savePath, bool pathIsFolder) noexcept
	{
		string meshSaveLocation = savePath;
		string prefabSavePath;
		if (!pathIsFolder)
		{
			prefabSavePath = savePath;
			for (string::iterator pathEnd = meshSaveLocation.end() - 1; pathEnd != meshSaveLocation.begin(); pathEnd--)
			{
				if (*pathEnd == '\\')
				{
					meshSaveLocation.erase(pathEnd + 1, meshSaveLocation.end());
					break;
				}
			}
		}
		else
		{
			prefabSavePath = StringBuilder(savePath, "Prefab", index, ".prefab").CStr();
		}

		json gameobject = CookHeirarchy(&tempMeshes, meshSaveLocation);

		ofstream output(prefabSavePath);
		output << std::setw(2) << gameobject;

		fileQueue.erase(fileQueue.begin() + index);
		tempMeshes.Clear();
		if (current > 0 && current >= fileQueue.size())
		{
			SetCurrent(current - 1);
		}
	}

	void ArtImporter::CookAll(string savePath) noexcept
	{
		while (fileQueue.size() > 0)
		{
			Preload(fileQueue[fileQueue.size() - 1].c_str());
			Cook(fileQueue.size() - 1, savePath, true);
		}
	}
}
