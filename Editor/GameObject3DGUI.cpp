#include "GameObject3DGUI.h"

#include "GUI.h"

void GameObject3DGUI::Draw()
{
	DrawGameObject3DGUI((GameObject3D*)target);
}

void GameObject3DGUI::DrawGameObject3DGUI(GameObject3D* gameObject3D)
{
	if (gameObject3D == nullptr || *gameObject3D == GameObject::Destroyed) return;

	GameObjectGUI::DrawGameObjectGUI(gameObject3D);

	std::string idStr = GUI::GenerateID("GameObject3D", gameObject3D);
	const char* id = idStr.c_str();

	if (ImGui::CollapsingHeader(id, ImGuiTreeNodeFlags_DefaultOpen))
	{
		vec3 position = gameObject3D->GetPosition();
		ImGui::DragFloat3("Position", (float*)&position, 0.25f);
		gameObject3D->SetPosition(position);


		vec3 rotationEuler = glm::eulerAngles(gameObject3D->GetRotation());
		vec3 rotationDegrees = vec3(glm::degrees(rotationEuler.x), glm::degrees(rotationEuler.y), glm::degrees(rotationEuler.z));

		ImGui::DragFloat3("Rotation", (float*)&rotationDegrees);

		rotationEuler = vec3(glm::radians(rotationDegrees.x), glm::radians(rotationDegrees.y), glm::radians(rotationDegrees.z));
		quat quatZ = glm::angleAxis(rotationEuler.z, vec3(0, 0, 1));
		quat quatY = glm::angleAxis(rotationEuler.y, vec3(0, 1, 0));
		quat quatX = glm::angleAxis(rotationEuler.x, vec3(1, 0, 0));

		gameObject3D->SetRotation(glm::normalize(quatZ * quatY * quatX));


		vec3 scale = gameObject3D->GetScale();
		ImGui::DragFloat3("Scale", (float*)&scale);
		gameObject3D->SetScale(scale);

		ImGui::Spacing(); ImGui::Spacing();

		vec3 pivot = gameObject3D->GetPivot();
		ImGui::DragFloat3("Pivot", (float*)&pivot, 0.5f);
		gameObject3D->SetPivot(pivot);

		ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
	}
}
