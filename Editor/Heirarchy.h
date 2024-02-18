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
	enum DragDropTargetOffset : char
	{
		ChildToFront = -2,
		ChildTo = -1,
		Before = 0,
		After = 1
	};

	inline void Draw();

	inline void DrawEntry(GameObject3D* gameObject3D);

	inline void DragDropTarget(const char* id, GameObject3D* target, DragDropTargetOffset offset = ChildTo);
}

void Heirarchy::Draw()
{
	if (ImGui::CollapsingHeader("2D"))
	{

		ImGui::Spacing();
	}

	if (ImGui::CollapsingHeader("3D"))
	{
		// Drag Drop Target BEFORE First GameObject3D
		for (GameObject* gameObject : gameObjectManager->gameObjects)
		{
			if (dynamic_cast<GameObject3D*>(gameObject) != nullptr)
			{
				ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
				DragDropTarget("GameObject3D", (GameObject3D*)gameObject, Before);
				break;
			}
		}
		
		// Draw 3D Heirarchy
		for (GameObject* gameObject : gameObjectManager->gameObjects)
		{
			if (dynamic_cast<GameObject3D*>(gameObject) != nullptr)
			{
				DrawEntry((GameObject3D*)gameObject);
			}
		}

		// Drag Drop Target AFTER Last GameObject3D
		for (int i = gameObjectManager->gameObjects.size() - 1; i >= 0; i--)
		{
			if (dynamic_cast<GameObject3D*>(gameObjectManager->gameObjects[i]) != nullptr)
			{
				ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
				DragDropTarget("GameObject3D", (GameObject3D*)gameObjectManager->gameObjects[i], After);
				break;
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

	if (gameObject3D->GetChildren()->size() == 0)
	{
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
		DragDropTarget("GameObject3D", gameObject3D, After);
	}
	else
	{
		ImGui::Indent();

		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
		DragDropTarget("GameObject3D", gameObject3D, ChildToFront);

		for (GameObject3D* child : *gameObject3D->GetChildren())
		{
			DrawEntry(child);
		}
		ImGui::Unindent();
	}

	ImGui::PopID();
}

void Heirarchy::DragDropTarget(const char* id, GameObject3D* target, DragDropTargetOffset offset)
{
	if (ImGui::BeginDragDropTarget())
	{
		const ImGuiPayload* payload = ImGui::GetDragDropPayload();

		if (payload->IsDataType("GameObject3D"))
		{
			GameObject3D** payloadObject = (GameObject3D**)payload->Data;

			if ((*payloadObject) != target && !(*payloadObject)->ContainsChild(target, true))
			{
				if (offset == ChildTo || offset == ChildToFront)
				{
					ImGui::AcceptDragDropPayload("GameObject3D");

					if (ImGui::IsKeyReleased(ImGuiKey_MouseLeft))
					{
						(*payloadObject)->SetParent(target);

						if (offset == ChildToFront)
						{
							target->MoveChildTo((*payloadObject), 0);
						}
					}
				}
				else
				{
					ImGui::AcceptDragDropPayload("GameObject3D");

					if (ImGui::IsKeyReleased(ImGuiKey_MouseLeft))
					{
						GameObject3D* cachedPayloadObject = *payloadObject;

						cachedPayloadObject->SetParent(target->GetParent());
						cachedPayloadObject->MoveTo(target->GetIndex() + offset);

					}
				}
			}
		}
		ImGui::EndDragDropTarget();
	}
}
