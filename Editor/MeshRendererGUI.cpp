#include "MeshRendererGUI.h"

#include "GUI.h"

#include "MeshManager.h"
#include "MaterialManager.h"

void MeshRendererGUI::Draw()
{
	DrawMeshRendererGUI((MeshRenderer*)target);
}

void MeshRendererGUI::DrawMeshRendererGUI(MeshRenderer* meshRenderer)
{
	if (meshRenderer == nullptr || meshRenderer == GameObject::Destroyed) return;

	GameObject3DGUI::DrawGameObject3DGUI(meshRenderer);

	std::string idStr = GUI::GenerateID(MeshRenderer::className, meshRenderer);
	const char* id = idStr.c_str();

	if (ImGui::CollapsingHeader(id, ImGuiTreeNodeFlags_DefaultOpen))
	{
		string meshFilepath;
		if (meshRenderer->GetMesh() == nullptr) meshFilepath = "None";
		else meshFilepath = meshRenderer->GetMesh()->GetFilePath();

		if (ImGui::BeginCombo("Mesh", meshFilepath.c_str()))
		{
			if (ImGui::Selectable("None", meshFilepath == "None"))
			{
				meshRenderer->SetMesh("None");
			}

			for (int i = 0; i < meshManager->defaultMeshes.size(); i++)
			{
				bool isCurrent = meshManager->defaultMeshes[i] == meshFilepath;
				if (ImGui::Selectable(meshManager->defaultMeshes[i].c_str(), isCurrent))
				{
					meshRenderer->SetMesh(meshManager->defaultMeshes[i].c_str());
				}
				if (isCurrent) ImGui::SetItemDefaultFocus();
			}

			if (ImGui::Selectable("Load From File"))
			{
				string loadPath = FileDialogue::GetLoadPath(FileDialogue::PathDetails("Load Mesh", "Assets\\", { "*.mesh" }), FileDialogue::LimitToAssetFolder::True);
				if (!loadPath.empty())
				{
					meshRenderer->SetMesh(loadPath.c_str());
				}
			}


			ImGui::EndCombo();
		}

		string materialFilepath = meshRenderer->GetMaterial()->GetFilePath();
		if (materialFilepath == "Engine\\DefaultAssets\\Default.mat") materialFilepath = "Default";

		if (ImGui::BeginCombo("Material", materialFilepath.c_str()))
		{
			for (int i = 0; i < materialManager->defaultMaterials.size(); i++)
			{
				bool isCurrent = materialManager->defaultMaterials[i] == materialFilepath;
				if (ImGui::Selectable(materialManager->defaultMaterials[i].c_str(), isCurrent))
				{
					meshRenderer->SetMaterial(materialManager->defaultMaterials[i].c_str());
				}
				if (isCurrent) ImGui::SetItemDefaultFocus();
			}

			if (ImGui::Selectable("Load From File"))
			{
				string loadPath = FileDialogue::GetLoadPath(FileDialogue::PathDetails("Load Material", "Assets\\", { "*.mat" }), FileDialogue::LimitToAssetFolder::True);
				if (!loadPath.empty())
				{
					meshRenderer->SetMaterial(loadPath.c_str());
				}
			}

			ImGui::EndCombo();
		}

		GUI::Spacing(3);
	}
}
