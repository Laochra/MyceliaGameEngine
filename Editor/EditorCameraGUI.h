#pragma once

#include "ImGuiIncludes.h"
#include "GUI.h"

#include "EditorCamera.h"
#include "EditorCameraConfig.h"
#include "InputEditor.h"

namespace EditorCameraGUI
{
	inline void Draw(EditorCamera* camera);
}

void EditorCameraGUI::Draw(EditorCamera* camera)
{
	std::string idStr = GUI::GenerateID("Editor Camera", camera);
	const char* id = idStr.c_str();

	if (ImGui::CollapsingHeader(id, ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::BeginTabBar("Camera Settings Tab Bar");
		{
			if (ImGui::BeginTabItem("Display"))
			{
				ImGui::SeparatorText("Clipping Planes");
				ImGui::DragFloat("Near", &camera->nearClip, 0.01f);
				ImGui::DragFloat("Far", &camera->farClip, 100.0f);

				ImGui::SeparatorText("FOV");
				float fovDegrees = glm::degrees(camera->fov);
				ImGui::DragFloat("(degrees)", &fovDegrees);
				camera->fov = glm::radians(fovDegrees);

				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Movement"))
			{
				ImGui::SeparatorText("Placement");
				vec3 position = camera->GetPosition();
				ImGui::DragFloat3("Position", (float*)&position);
				camera->SetPosition(position);

				vec2 rotation = vec2(camera->xRotation, camera->yRotation);
				ImGui::DragFloat2("Rotation", (float*)&rotation);
				camera->xRotation = rotation.x;
				camera->yRotation = rotation.y;

				ImGui::SeparatorText("Fly Speed");
				ImGui::DragFloat("Regular", &(camera)->flySpeed);
				ImGui::DragFloat("Quick", &(camera)->quickFlySpeed);

				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Controls"))
			{
				ImGui::SeparatorText("Movement");

				InputEditor::ConfigureKeyAxis("Back", "Forward", &(camera)->zInput);
				ImGui::Spacing(); ImGui::Spacing();
				InputEditor::ConfigureKeyAxis("Left", "Right", &(camera)->xInput);
				ImGui::Spacing(); ImGui::Spacing();
				InputEditor::ConfigureKeyAxis("Down", "Up", &(camera)->yInput);

				ImGui::SeparatorText("Other");

				InputEditor::ConfigureKeybind("Quick Fly", &(camera)->quickMode);
				ImGui::Spacing(); ImGui::Spacing();
				InputEditor::ConfigureKeybind("Free Camera", &(camera)->freeCamera);

				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();

		ImGui::Spacing(); ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing(); ImGui::Spacing();

		if (ImGui::Button("Save Settings"))
		{
			EditorCameraConfig::Save(camera);
		}
	}
}