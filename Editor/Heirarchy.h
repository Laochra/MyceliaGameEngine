#pragma once

#include "ImGuiIncludes.h"
#include "imgui_stdlib.h"
#include "GUI.h"
#include "GameObjectManager.h"
//#include "GameObject2D.h"
#include "GameObject3D.h"

#include <vector>
using std::vector;

#include <string>
using std::string;

#include "ConsoleGUI.h"

namespace Heirarchy
{
	inline void Draw();

	inline void DrawEntry(GameObject3D* gameObject3D);

	inline void DragDropTarget(const char* id, GameObject3D* target, int offset = -1);
}

void Heirarchy::Draw()
{
	if (ImGui::CollapsingHeader("2D"))
	{

		ImGui::Spacing();
	}

	if (ImGui::CollapsingHeader("3D"))
	{
		for (GameObject* gameObject : gameObjectManager->gameObjects)
		{
			if (dynamic_cast<GameObject3D*>(gameObject) != nullptr)
			{
				DrawEntry((GameObject3D*)gameObject);
			}
		}

		ImGui::Spacing();
	}
}

void Heirarchy::DrawEntry(GameObject3D* gameObject3D)
{
	ImGui::PushID(GUI::GenerateID(gameObject3D).c_str());

	ImGui::Text(gameObject3D->GetName());
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
	{
		ImGui::SetDragDropPayload("GameObject3D", &gameObject3D, sizeof(GameObject3D**));
		if (ImGui::IsDragDropPayloadBeingAccepted())
		{
			ImGui::Text("Move GameObject3D here?");
		}
		else
		{
			ImGui::TextColored({ 255, 0, 0, 1 }, "Invalid target");
		}
		ImGui::EndDragDropSource();
	}
	DragDropTarget("GameObject3D", gameObject3D);

	ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
	DragDropTarget("GameObject3D", gameObject3D, 1);

	if (gameObject3D->GetChildren()->size() != 0)
	{
		ImGui::Indent();
		for (GameObject3D* child : *gameObject3D->GetChildren())
		{
			DrawEntry(child);
		}
		ImGui::Unindent();
	}

	ImGui::PopID();
}

void Heirarchy::DragDropTarget(const char* id, GameObject3D* target, int offset)
{
	if (ImGui::BeginDragDropTarget())
	{
		const ImGuiPayload* payload = ImGui::GetDragDropPayload();

		if (payload->IsDataType("GameObject3D"))
		{
			GameObject3D** payloadObject = (GameObject3D**)payload->Data;

			if (!(*payloadObject)->ContainsChild(target, true))
			{
				if (offset == -1)
				{
					ImGui::AcceptDragDropPayload("GameObject3D");

					if (ImGui::IsKeyReleased(ImGuiKey_MouseLeft))
					{
						(*payloadObject)->SetParent(target);
					}
				}
				else
				{
					ImGui::AcceptDragDropPayload("GameObject3D");

					if (ImGui::IsKeyReleased(ImGuiKey_MouseLeft))
					{
						(*payloadObject)->SetParent(target->GetParent());
						(*payloadObject)->MoveTo(target->GetIndex() + offset);
					}
				}
			}
		}
		ImGui::EndDragDropTarget();
	}
}
