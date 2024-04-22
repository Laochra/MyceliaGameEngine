#include "LightObjectGUI.h"

#include "GUI.h"

#include "Debug.h"

void LightObjectGUI::Draw()
{
	DrawLightObjectGUI((LightObject*)target);
}

void LightObjectGUI::DrawLightObjectGUI(LightObject* lightObject)
{
	if (lightObject == nullptr || *lightObject == GameObject::Destroyed) return;

	GameObject3DGUI::DrawGameObject3DGUI(lightObject);

	std::string idStr = GUI::GenerateID("LightObject", lightObject);
	const char* id = idStr.c_str();

	if (ImGui::CollapsingHeader(id, ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::ColorEdit3("Colour", (float*)&lightObject->colour);
		ImGui::DragFloat("Intensity", &lightObject->intensity, 0.01f, 0.0f, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp);
		
		GUI::Spacing(3);

		ImGui::DragFloat("Range", &lightObject->range, 0.1f, 0.0f, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp);
		float angleEulerDegrees = glm::degrees(glm::acos(lightObject->angle));
		if (ImGui::DragFloat("Angle", &angleEulerDegrees, 0.5f, 0.0f, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp))
		{
			lightObject->angle = glm::cos(glm::radians(angleEulerDegrees));
			debug->Log(std::to_string(lightObject->angle));
		}
		if (ImGui::IsItemHovered())
		{
			if (ImGui::BeginTooltip())
			{
				ImGui::Text("Leave angle as 0 for point light");
				ImGui::Text("Values other than 0 are WIP!");
				ImGui::EndTooltip();
			}
		}

		GUI::Spacing(3);
	}
}
