#pragma once

#include "ImGuiIncludes.h"
#include "GUI.h"
#include "GameObject3DGUI.h"

#include "MeshRenderer.h"

namespace MeshRendererGUI
{
	inline void Draw(MeshRenderer* meshRenderer);
}

void MeshRendererGUI::Draw(MeshRenderer* meshRenderer)
{
	GameObject3DGUI::Draw(meshRenderer);

	std::string idStr = GUI::GenerateID("Mesh Renderer", meshRenderer);
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