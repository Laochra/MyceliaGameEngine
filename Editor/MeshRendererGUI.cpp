#include "MeshRendererGUI.h"

#include "GUI.h"

#include "MaterialGUI.h"

void MeshRendererGUI::Draw()
{
	DrawMeshRendererGUI((MeshRenderer*)target);
}

void MeshRendererGUI::DrawMeshRendererGUI(MeshRenderer* meshRenderer)
{
	if (meshRenderer == nullptr || *meshRenderer == GameObject::Destroyed) return;

	GameObject3DGUI::DrawGameObject3DGUI(meshRenderer);

	std::string idStr = GUI::GenerateID("MeshRenderer", meshRenderer);
	const char* id = idStr.c_str();

	if (ImGui::CollapsingHeader(id, ImGuiTreeNodeFlags_DefaultOpen))
	{
		string meshFilepath;
		if (meshRenderer->GetMesh() == nullptr) meshFilepath = "None";
		else meshFilepath = meshRenderer->GetMesh()->GetFilePath();

		vector<path> meshes;
		for (const directory_entry& entry : directory_iterator("Assets\\Meshes"))
		{
			path filePath = entry.path();
			string extension = filePath.extension().string();

			if (extension == ".obj" || extension == ".fbx" || extension == ".gltf") meshes.push_back(filePath);
		}

		if (ImGui::BeginCombo("Mesh", meshFilepath.c_str()))
		{
			if (ImGui::Selectable("None", meshFilepath == "None"))
			{
				meshRenderer->SetMesh("None");
			}

			for (int i = 0; i < meshes.size(); i++)
			{
				bool isCurrent = meshes[i].string() == meshFilepath;
				if (ImGui::Selectable(meshes[i].filename().string().c_str(), isCurrent))
				{
					meshRenderer->SetMesh(meshes[i].string().c_str());
				}
				if (isCurrent) ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}
	}

	MaterialGUI::Initialise();

	string materialFilepath = meshRenderer->GetMaterial()->GetFilePath();
	if (materialFilepath == "Engine\\DefaultAssets\\Default.mat") materialFilepath = "Default";

	if (ImGui::BeginCombo("Material", materialFilepath.c_str()))
	{
		if (ImGui::Selectable("Default", materialFilepath == "Default"))
		{
			meshRenderer->SetMaterial("Default");
		}

		for (int i = 0; i < MaterialGUI::materials.size(); i++)
		{
			bool isCurrent = MaterialGUI::materials[i].string() == materialFilepath;
			if (ImGui::Selectable(MaterialGUI::materials[i].filename().string().c_str(), isCurrent))
			{
				meshRenderer->SetMaterial(MaterialGUI::materials[i].string().c_str());
			}
			if (isCurrent) ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}
}
