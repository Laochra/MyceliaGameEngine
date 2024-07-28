#include "CameraGUI.h"

#include "GUI.h"

void CameraGUI::Draw()
{
	DrawCameraGUI((Camera*)target);
}

void CameraGUI::DrawCameraGUI(Camera* camera)
{
	if (camera == nullptr || camera == GameObject::Destroyed) return;

	GameObject3DGUI::DrawGameObject3DGUI(camera);

	std::string idStr = GUI::GenerateID(Camera::className, camera);
	const char* id = idStr.c_str();

	if (ImGui::CollapsingHeader(id, ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::SeparatorText("Clipping Planes");
		if (ImGui::DragFloat("Near", &camera->nearClip, 0.01f, 0.01f, FLT_MAX, "%.3f", ImGuiSliderFlags_AlwaysClamp))
		{
			if (camera->nearClip >= camera->farClip) camera->farClip = camera->nearClip + FLT_MIN;
		}
		if (ImGui::DragFloat("Far", &camera->farClip, 100.0f, 0.01f, FLT_MAX, "%.3f", ImGuiSliderFlags_AlwaysClamp))
		{
			if (camera->farClip <= camera->nearClip) camera->nearClip = camera->farClip - 0.01f;
		}

		ImGui::SeparatorText("FOV");
		float fovDegrees = glm::degrees(camera->fov);
		if (ImGui::DragFloat("(degrees)", &fovDegrees, 0.25f, 0.25f, 179.75f, "%.3f", ImGuiSliderFlags_AlwaysClamp))
		{
			camera->fov = glm::radians(fovDegrees);
		}

		GUI::Spacing(3);
	}
}
