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
		bool angleIs0 = lightObject->angle[1] == glm::cos(glm::radians(0.0f));

		string preview = angleIs0 ? "Point Light" : "Spotlight";
		if (ImGui::BeginCombo("Type", preview.c_str()))
		{
			if (ImGui::Selectable("Point Light", angleIs0))
			{
				lightObject->angle = vec2(glm::cos(glm::radians(0.0f)));
			}
			if (ImGui::Selectable("Spotlight", !angleIs0))
			{
				if (angleIs0) lightObject->angle = vec2(glm::cos(glm::radians(0.0f)), glm::cos(glm::radians(25.0f)));
			}
			ImGui::EndCombo();
		}

		GUI::Spacing(1);

		ImGui::SeparatorText("HDR Colour");

		ImGui::ColorEdit3("Colour", (float*)&lightObject->colour);
		ImGui::DragFloat("Intensity", &lightObject->intensity, 0.1f, 0.1f, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp);
		
		GUI::Spacing(1);

		ImGui::SeparatorText("Range");

		ImGui::DragFloat("Distance", &lightObject->range, 0.1f, 0.1f, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp);

		if (!angleIs0)
		{
			GUI::Spacing(1);
			vec2 angleEulerDegrees = glm::degrees(glm::acos(lightObject->angle));
			if (ImGui::DragFloat("Inner Angle", &angleEulerDegrees[0], 0.1f, 0.0f, angleEulerDegrees[1], NULL, ImGuiSliderFlags_AlwaysClamp))
			{
				lightObject->angle[0] = glm::cos(glm::radians(angleEulerDegrees[0]));
			}
			if (ImGui::DragFloat("Outer Angle", &angleEulerDegrees[1], 0.1f, 0.1f, 89.9f, NULL, ImGuiSliderFlags_AlwaysClamp))
			{
				lightObject->angle[1] = glm::cos(glm::radians(angleEulerDegrees[1]));
				if (angleEulerDegrees[0] > angleEulerDegrees[1])
				{
					lightObject->angle[0] = glm::cos(glm::radians(angleEulerDegrees[1]));
				}
			}
		}

		GUI::Spacing(3);
	}
}
