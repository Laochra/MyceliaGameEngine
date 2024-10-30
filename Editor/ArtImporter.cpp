#include "ArtImporter.h"

#include "MeshRenderer.h"
#include "MeshManager.h"
#include "Camera.h"
#include "Shader.h"
#include "PostProcessing.h"
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
	bool initialised = false;

	vector<string> fileQueue;
	int current;

	bool compressHeirarchy = true;
	bool applyTransforms = false;

	MeshHeirarchy tempMeshes;
	MeshHeirarchy* selectedTempMesh;

	Assimp::Importer importer;
	const aiScene* currentFile;

	uint hdrFBO = 0;
	uint hdrColour = 0;
	uint hdrDepth = 0;
	ShaderProgram sp;
	uint previewFBO = 0;
	uint previewColour = 0;
	vec2 previewSize;
	Camera* previewCamera = nullptr;
	vec3 previewBg;

	GameObject3D* previewObject = nullptr;
	GameObject3D* selectedObject = nullptr;
	void UpdateObjectPreview();

	static void Initialise() noexcept
	{
		initialised = true;

		previewCamera = GameObject::Instantiate<Camera>();
		gameObjectManager->Remove(previewCamera);
		previewCamera->fov = glm::radians(80.0f);
		previewCamera->SetPosition(vec3(0, 0, 1));
		previewCamera->LookAt(vec3());


		sp.LoadShader(VertexStage, "Engine\\DefaultAssets\\FullScreenQuad.vert");
		sp.LoadShader(FragmentStage, "Engine\\DefaultAssets\\BasicHDR.frag");
		sp.Link();

		// HDR Buffer Setup
		glGenFramebuffers(1, &hdrFBO);
		glGenRenderbuffers(1, &hdrDepth);
		glGenTextures(1, &hdrColour);
		glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

		glBindRenderbuffer(GL_RENDERBUFFER, hdrDepth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, (int)previewSize.x, (int)previewSize.y);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, hdrDepth);

		glBindTexture(GL_TEXTURE_2D, hdrColour);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, (int)previewSize.x, (int)previewSize.y, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hdrColour, 0);


		// Preview Buffer Setup
		glGenFramebuffers(1, &previewFBO);
		glGenTextures(1, &previewColour);
		glBindFramebuffer(GL_FRAMEBUFFER, previewFBO);

		glBindTexture(GL_TEXTURE_2D, previewColour);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (int)previewSize.x, (int)previewSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, previewColour, 0);
	}
	static void RefreshPreviewSize() noexcept
	{
		glBindRenderbuffer(GL_RENDERBUFFER, hdrDepth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, (int)previewSize.x, (int)previewSize.y);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		
		glBindTexture(GL_TEXTURE_2D, hdrColour);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, (int)previewSize.x, (int)previewSize.y, 0, GL_RGBA, GL_FLOAT, NULL);
		glBindTexture(GL_TEXTURE_2D, 0);

		glBindTexture(GL_TEXTURE_2D, previewColour);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (int)previewSize.x, (int)previewSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	static void DrawObjects(GameObject3D* object)
	{
		object->Draw();
		for (GameObject3D* child : *object->GetChildren())
		{
			DrawObjects(child);
		}
	}
	static void DrawPreview() noexcept
	{
		if (previewObject == nullptr) return;

		Camera* dormantCamera = AppInfo::ActiveCamera();
		AppInfo::ActiveCamera() = previewCamera;
		glm::ivec2 dormantScreensize(AppInfo::screenWidth, AppInfo::screenHeight);
		AppInfo::screenWidth = previewSize.x;
		AppInfo::screenHeight = previewSize.y;
		previewCamera->Update();
		glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
		glViewport(0, 0, previewSize.x, previewSize.y);
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		DrawObjects(previewObject);

		glBindFramebuffer(GL_FRAMEBUFFER, previewFBO);
		glViewport(0, 0, previewSize.x, previewSize.y);
		glClearColor(previewBg.r, previewBg.g, previewBg.b, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		Mesh screenQuad;
		screenQuad.InitialiseQuad();

		sp.Bind();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, hdrColour);
		sp.BindUniform("HDRTexture", 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, previewColour);
		sp.BindUniform("CurrentColourBuffer", 1);
		sp.BindUniform("Exposure", PostProcess::Defaults::exposure);

		screenQuad.Draw();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		AppInfo::ActiveCamera() = dormantCamera;
		AppInfo::screenWidth = dormantScreensize.x;
		AppInfo::screenHeight = dormantScreensize.y;
	}

	static void SetCurrent(int newIndex)
	{
		if (newIndex < fileQueue.size() || newIndex == 0) { current = newIndex; }
		else { current = (int)fileQueue.size() - 1; }

		if (fileQueue.size() > 0) { Preload(fileQueue[current].c_str()); }
	}

	static void DrawNode(MeshHeirarchy* tempMesh, GameObject3D* object)
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
					if (selectedObject != nullptr) selectedObject->selected = false;
					selectedObject = object;
					selectedObject->selected = true;
				}
			}
			for (int i = 0; i < tempMesh->children.size(); i++)
			{
				DrawNode(tempMesh->children[i], (*object->GetChildren())[i]);
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
					selectedObject = object;
				}
			}
		}
	}

	void ArtImporter::Draw(const char* const name, bool& open) noexcept
	{
		if (!ArtImporter::initialised) ArtImporter::Initialise();

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
			if (ImGui::Button(" < ")) { SetCurrent(current - 1); selectedTempMesh = nullptr; selectedObject = nullptr; }
			ImGui::EndDisabled();
		}
		ImGui::SameLine();
		ImGui::Text((std::to_string(fileQueue.size() == 0 ? 0 : current + 1) + "/" + std::to_string(fileQueue.size())).c_str());
		ImGui::SameLine();
		{
			ImGui::BeginDisabled(fileQueue.size() == 0 || current >= fileQueue.size() - 1);
			if (ImGui::Button(" > ")) { SetCurrent(current + 1); selectedTempMesh = nullptr; selectedObject = nullptr; }
			ImGui::EndDisabled();
		}

		if (ImGui::Checkbox("Compress Heirarchy", &compressHeirarchy))
		{
			if (fileQueue.size() > 0)
			{
				Preload(fileQueue[current].c_str());
			}
		}
		if (ImGui::Checkbox("Respect Transform Data", &applyTransforms))
		{
			if (fileQueue.size() > 0)
			{
				Preload(fileQueue[current].c_str());
			}
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
				UpdateObjectPreview();
				SetCurrent(0);
				selectedTempMesh = nullptr;
				selectedObject = nullptr;
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
				selectedObject = nullptr;
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

			GUI::Spacing(3);

			float windowWidth = ImGui::GetWindowWidth();
			float windowHeight = ImGui::GetWindowHeight();
			if (ImGui::CollapsingHeader("Camera Settings"))
			{
				vec3 position = previewCamera->GetPosition();
				ImGui::DragFloat3("Position", (float*)&position, 0.25f);
				previewCamera->SetPosition(position);

				vec3 rotationEuler = glm::eulerAngles(previewCamera->GetRotationQuat());
				vec3 rotationDegrees = vec3(glm::degrees(rotationEuler.x), glm::degrees(rotationEuler.y), glm::degrees(rotationEuler.z));
				vec3 originalDegrees = rotationDegrees;
				if (ImGui::DragFloat3("Rotation", (float*)&rotationDegrees))
				{
					rotationDegrees -= originalDegrees;
					rotationEuler = vec3(glm::radians(rotationDegrees.x), glm::radians(rotationDegrees.y), glm::radians(rotationDegrees.z));

					quat quatZ = glm::angleAxis(rotationEuler.z, vec3(0, 0, 1));
					quat quatY = glm::angleAxis(rotationEuler.y, vec3(0, 1, 0));
					quat quatX = glm::angleAxis(rotationEuler.x, vec3(1, 0, 0));

					previewCamera->SetRotation(glm::normalize(quatZ * quatY * quatX) * previewCamera->GetRotationQuat());
				}

				GUI::Spacing();

				ImGui::PushItemWidth(ImGui::CalcItemWidth() / 3.025f);
				if (ImGui::DragFloat("##Near", &previewCamera->nearClip, 0.01f, 0.01f, FLT_MAX, "%.2f", ImGuiSliderFlags_AlwaysClamp))
				{
					if (previewCamera->nearClip >= previewCamera->farClip) previewCamera->farClip = previewCamera->nearClip + FLT_MIN;
				}
				ImGui::SameLine();
				if (ImGui::DragFloat("Near/Far Clip##Far", &previewCamera->farClip, 100.0f, 0.01f, FLT_MAX, "%.2f", ImGuiSliderFlags_AlwaysClamp))
				{
					if (previewCamera->farClip <= previewCamera->nearClip) previewCamera->nearClip = previewCamera->farClip - 0.01f;
				}

				float fovDegrees = glm::degrees(previewCamera->fov);
				if (ImGui::DragFloat("FOV", &fovDegrees, 0.25f, 0.25f, 179.75f, "%.2f", ImGuiSliderFlags_AlwaysClamp))
				{
					previewCamera->fov = glm::radians(fovDegrees);
				}
				ImGui::PopItemWidth();

				GUI::Spacing(3);
			}
			if (ImGui::CollapsingHeader("Preview", ImGuiTreeNodeFlags_DefaultOpen))
			{
				vec2 newSize = vec2(windowWidth * 0.95f, windowHeight * 0.25f);
				if (previewSize != newSize)
				{
					previewSize = newSize;
					RefreshPreviewSize();
				}
				DrawPreview();
				uintptr_t image = (uintptr_t)previewColour;
				ImGui::Image((ImTextureID)image, *(ImVec2*)&previewSize, ImVec2(0, 1), ImVec2(1, 0));
				
				GUI::Spacing(3);

				ImGui::ColorEdit3("Background", (float*)&previewBg, ImGuiColorEditFlags_NoInputs);
			}
			if (ImGui::CollapsingHeader("Heirarchy", ImGuiTreeNodeFlags_DefaultOpen))
			{
				if (ImGui::BeginChild("HeirarchyBox", ImVec2(0, windowHeight * 0.25f)))
				{
					DrawNode(&tempMeshes, previewObject);
				}
				ImGui::EndChild();

				ImGui::SeparatorText("Node Settings");
				if (selectedTempMesh == nullptr)
				{
					ImGui::Text("Select a node to edit it");
				}
				else
				{
					if (ImGui::InputText("Name", &selectedTempMesh->name))
					{
						if (selectedTempMesh->name.size() == 0) selectedTempMesh->name = "Empty";
					}

					if (selectedTempMesh->filepath != "None")
					{
						if (ImGui::BeginCombo("Material", selectedTempMesh->materialPath.c_str()))
						{
							if (ImGui::Selectable("Default", selectedTempMesh->materialPath == "Default"))
							{
								selectedTempMesh->materialPath = "Default";
								UpdateObjectPreview();
							}
							if (ImGui::Selectable("Select from file"))
							{
								using namespace FileDialogue;
								string newFilepath = GetLoadPath(PathDetails("Select Material", "Assets\\Materials\\", { "*.mat" }), LimitToAssetFolder::True);

								if (newFilepath.size() != 0)
								{
									selectedTempMesh->materialPath = newFilepath;
									UpdateObjectPreview();
								}
							}
							ImGui::EndCombo();
						}
					}
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

	void UpdateObjectPreviewNode(const MeshHeirarchy* heirarchyNode, GameObject3D*& object)
	{
		if (heirarchyNode->filepath == "None")
		{
			object = GameObject3D::Instantiate<GameObject3D>(heirarchyNode->localMatrix);
		}
		else
		{
			object = GameObject3D::Instantiate<MeshRenderer>(heirarchyNode->localMatrix);
			((MeshRenderer*)object)->SetMesh(heirarchyNode->filepath.c_str());
			((MeshRenderer*)object)->SetMaterial(heirarchyNode->materialPath.c_str());
			Updater::DrawRemove(object);
		}

		for (MeshHeirarchy* childNode : heirarchyNode->children)
		{
			GameObject3D* child = nullptr;
			UpdateObjectPreviewNode(childNode, child);
			child->SetParent(object);
		}
	}
	void UpdateObjectPreview()
	{
		if (previewObject != nullptr)
		{
			gameObjectManager->Add(previewObject);
			gameObjectManager->Delete(previewObject);
		}

		UpdateObjectPreviewNode(&tempMeshes, previewObject);
		
		gameObjectManager->Remove(previewObject);
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
		selectedTempMesh = nullptr;
		selectedObject = nullptr;
		ProcessNode(currentFile->mRootNode, tempMeshes);
		UpdateObjectPreview();
	}

	static json CookHeirarchy(MeshHeirarchy* mesh, string saveLocation)
	{
		string newPath;

		bool hasMesh = mesh->filepath != "None";

		// Move Mesh
		if (hasMesh)
		{
			newPath = saveLocation + string(mesh->name) + ".mesh";
			std::rename(mesh->filepath.c_str(), newPath.c_str());
		}

		// Construct Object for Prefab
		json object;

		object["Name"] = mesh->name;
		object["GUID"] = GuidGenerator::NewGuid();
		object["Active"] = true;
		object["TypeID"] = hasMesh ? MeshRenderer::classID : GameObject3D::classID;
		const mat4& t = mesh->localMatrix;
		const vec3 position(t[3]);
		object["Position"] = vector{ position.x, position.y, position.z };
		const vec3 scale(glm::length((vec3)t[0]), glm::length((vec3)t[1]), glm::length((vec3)t[2]));
		object["Scale"] = vector{ scale.x, scale.y, scale.z };
		const mat3 rotMat((vec3)t[0] / scale[0], (vec3)t[1] / scale[1], (vec3)t[2] / scale[2]);
		const quat rotation = glm::quat_cast(rotMat);
		object["Rotation"] = vector{ rotation.w, rotation.x, rotation.y, rotation.z };
		object["Pivot"] = vector{ 0.0f, 0.0f, 0.0f };
		if (hasMesh)
		{
			object["Mesh"] = newPath;
			object["Material"] = mesh->materialPath;
		}

		vector<json> children;
		for (int i = 0; i < mesh->children.size(); i++)
		{
			children.push_back(CookHeirarchy(mesh->children[i], saveLocation));
		}
		object["Children"] = children;

		return object;
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
		UpdateObjectPreview();
		if (current > 0 && current >= fileQueue.size())
		{
			SetCurrent(current - 1);
		}

		meshManager->ReloadAll();
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
