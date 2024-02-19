#include "MeshRendererGUI.h"

#include "GUI.h"

void MeshRendererGUI::Draw()
{
	DrawMeshRendererGUI((MeshRenderer*)target);
}

void MeshRendererGUI::DrawMeshRendererGUI(MeshRenderer* meshRenderer)
{
	GameObject3DGUI::DrawGameObject3DGUI(meshRenderer);

	std::string idStr = GUI::GenerateID("MeshRenderer", meshRenderer);
	const char* id = idStr.c_str();

	if (ImGui::CollapsingHeader(id, ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (meshRenderer->GetTexture() != nullptr)
		{
			ImGui::Text("Texture Filename:");
			ImGui::SameLine();
			ImGui::Text(meshRenderer->GetTexture()->GetFileName().c_str());
		}
		else
		{
			ImGui::Text("Texture Filename: -");
		}

		ImGui::Text("Mesh Filename: -");

		ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
	}
}
