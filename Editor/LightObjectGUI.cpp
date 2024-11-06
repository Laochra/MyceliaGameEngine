#include "LightObjectGUI.h"

#include "GUI.h"

#include "Debug.h"

void LightObjectGUI::Draw()
{
	DrawLightObjectGUI((LightObject*)target);
}

void LightObjectGUI::DrawLightObjectGUI(LightObject* lightObject)
{
	if (lightObject == nullptr || lightObject == GameObject::Destroyed) return;

	GameObject3DGUI::DrawGameObject3DGUI(lightObject);

	std::string idStr = GUI::GenerateID(LightObject::className, lightObject);
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

				// Shadows for point lights are currently disabled!
				lightObject->shadowMode = NoShadows;
				lightObject->shadowMapCount = 0U;
				for (uint i = 0; i < 6U; i++)
				{
					if (lightObject->shadowMaps[i] != 0U)
					{
						glDeleteTextures(1, &lightObject->shadowMaps[i]);
						lightObject->shadowMaps[i] = 0U;
					}
				}

				//if (lightObject->shadowMode)
				//{
				//	lightObject->shadowMapCount = 6U;
				//}
			}
			if (ImGui::Selectable("Spotlight", !angleIs0))
			{
				if (angleIs0) lightObject->angle = vec2(glm::cos(glm::radians(0.0f)), glm::cos(glm::radians(25.0f)));

				if (lightObject->shadowMode)
				{
					lightObject->shadowMapCount = 1U;
					for (uint i = 1; i < 6U; i++)
					{
						if (lightObject->shadowMaps[i] != 0U)
						{
							glDeleteTextures(1, &lightObject->shadowMaps[i]);
							lightObject->shadowMaps[i] = 0U;
						}
					}
				}
			}
			ImGui::EndCombo();
		}

		GUI::Spacing(1);

		const char* shadowModeName;
		switch (lightObject->shadowMode)
		{
		case NoShadows: shadowModeName = "No Shadows"; break;
		case HardShadows: shadowModeName = "Hard Shadows"; break;
		case SoftShadows: shadowModeName = "Soft Shadows"; break;
		default: shadowModeName = "Invalid Option Selected"; break;
		}

		ImGui::BeginDisabled(angleIs0);
		if (ImGui::BeginCombo("Shadow Mode", shadowModeName))
		{
			if (ImGui::Selectable("No Shadows", lightObject->shadowMode == NoShadows))
			{
				lightObject->shadowMode = NoShadows;
				lightObject->shadowMapCount = 0U;
				for (uint i = 0; i < 6U; i++)
				{
					if (lightObject->shadowMaps[i] != 0U)
					{
						glDeleteTextures(1, &lightObject->shadowMaps[i]);
						lightObject->shadowMaps[i] = 0U;
					}
				}
			}
			if (ImGui::Selectable("Hard Shadows", lightObject->shadowMode == HardShadows))
			{
				lightObject->shadowMode = HardShadows;
				lightObject->shadowMapCount = angleIs0 ? 6U : 1U;
			}
			if (ImGui::Selectable("Soft Shadows", lightObject->shadowMode == SoftShadows))
			{
				lightObject->shadowMode = SoftShadows;
				lightObject->shadowMapCount = angleIs0 ? 6U : 1U;
			}
			ImGui::EndCombo();
		}
		ImGui::EndDisabled();

		if (lightObject->shadowMode != NoShadows)
		{
			if (ImGui::DragFloat("Near Clip", &lightObject->nearClip, 0.01f, 0.01f, FLT_MAX))
			{
				if (lightObject->nearClip >= lightObject->farClip)
				{
					lightObject->farClip = lightObject->nearClip + 0.01f;
				}
			}
			if (ImGui::DragFloat("Far Clip", &lightObject->farClip, 1.0f, 1.0f, FLT_MAX))
			{
				if (lightObject->farClip <= lightObject->nearClip)
				{
					lightObject->nearClip = lightObject->farClip - 0.01f;
				}
			}
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
