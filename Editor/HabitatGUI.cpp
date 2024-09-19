#include "HabitatGUI.h"

#include "GUI.h"

void HabitatGUI::Draw()
{
	DrawHabitatGUI((HabitatObject*)target);
}

void HabitatGUI::DrawHabitatGUI(HabitatObject* habitat)
{
	if (habitat == nullptr || habitat == GameObject::Destroyed) return;

	MeshRendererGUI::DrawMeshRendererGUI(habitat);

	std::string idStr = GUI::GenerateID(HabitatObject::className, habitat);
	const char* id = idStr.c_str();

	if (ImGui::CollapsingHeader(id, ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("Habitat inspector stuff :D");
	}
}
