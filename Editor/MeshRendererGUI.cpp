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
		MaterialGUI::Initialise();

		string currentFilepath = meshRenderer->GetMaterial()->GetFilePath();
		if (currentFilepath == "Engine\\DefaultAssets\\Default.mat") currentFilepath = "Default";

		if (ImGui::BeginCombo("Material", currentFilepath.c_str()))
		{
			if (ImGui::Selectable("Default", currentFilepath == "Default"))
			{
				meshRenderer->SetMaterial("Default");
			}

			for (int i = 0; i < MaterialGUI::materials.size(); i++)
			{
				bool isCurrent = MaterialGUI::materials[i].string() == currentFilepath;
				if (ImGui::Selectable(MaterialGUI::materials[i].filename().string().c_str(), isCurrent))
				{
					meshRenderer->SetMaterial(MaterialGUI::materials[i].string().c_str());
				}
				if (isCurrent) ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		ImGui::Text("Mesh:");
		ImGui::SameLine();
		ImGui::Text(meshRenderer->GetMesh()->GetFilePath());
	}
}
