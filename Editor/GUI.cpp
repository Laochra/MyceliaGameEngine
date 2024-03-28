#include "GUI.h"

#include <sstream>

#include "GameObject.h"

GUI::EditorColours GUI::colours;

std::string GUI::GenerateID(GameObject* gameObject)
{
	std::ostringstream stream;
	stream << gameObject->name << "##" << gameObject->GetGUID();

	return stream.str();
}

std::string GUI::GenerateID(const char* name, GameObject* gameObject)
{
	std::ostringstream stream;
	stream << name << "##" << gameObject->GetGUID();

	return stream.str();
}

void GUI::Spacing(uint numberOfSpacings)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return;

	for (uint i = 0; i < numberOfSpacings; i++)
	{
		ImGui::ItemSize(ImVec2(0, 0));
	}
}
