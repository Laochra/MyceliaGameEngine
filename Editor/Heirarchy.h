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

#include "Inspector.h"

namespace Heirarchy
{
	enum DragDropTargetOffset : char
	{
		ChildToFront = -2,
		ChildTo = -1,
		Before = 0,
		After = 1
	};

	struct RightClickMenu
	{
		bool open = false;
		ImVec2 position = ImVec2(0.0f, 0.0f);
		GameObject* target = nullptr;

		void Open(GameObject* targetInit, ImVec2 positionInit)
		{
			open = true;
			position = positionInit;
			target = targetInit;
		}

		void Close()
		{
			open = false;
			target = nullptr;
		}
	};

	inline RightClickMenu rightClickMenu;

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
		for (int i = (int)gameObjectManager->gameObjects.size() - 1; i >= 0; i--)
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

	if (rightClickMenu.open)
	{
		ImGui::SetNextWindowSize(ImVec2(150, 75));
		ImGui::SetNextWindowPos(rightClickMenu.position);
		ImGui::SetNextWindowBgAlpha(0.9f);

		ImGui::Begin(rightClickMenu.target->GetName(), (bool*)0, ImGuiWindowFlags_NoMove || ImGuiWindowFlags_NoDecoration);
		
		bool isHovered = false;

		if (ImGui::BeginMenu("Add New"))
		{
			if (ImGui::IsWindowHovered()) isHovered = true;

			if (dynamic_cast<GameObject3D*>(rightClickMenu.target) != nullptr)
			{
				if (ImGui::MenuItem(GameObject3D::className))
				{
					GameObject3D::Instantiate<GameObject3D>((GameObject3D*)rightClickMenu.target); rightClickMenu.Close();
				}
				if (ImGui::MenuItem(MeshRenderer::className))
				{
					GameObject3D::Instantiate<MeshRenderer>((GameObject3D*)rightClickMenu.target); rightClickMenu.Close();
				}
				if (ImGui::MenuItem(LightObject::className)) 
				{
					GameObject3D::Instantiate<LightObject>((GameObject3D*)rightClickMenu.target); rightClickMenu.Close();
				}
			}
			ImGui::EndMenu();
		}
		if (ImGui::MenuItem("Duplicate"))
		{
			GameObject3D* original = (GameObject3D*)rightClickMenu.target;
			GameObject3D* parent = original->GetParent();

			json prefab = rightClickMenu.target;

			GameObject3D* clone = (GameObject3D*)GameObject::InstantiateFrom(prefab, GuidGeneration::New);
			gameObjectManager->Add(clone);
			clone->SetParent(parent);
			clone->MoveTo(original->GetIndex() + 1);

			rightClickMenu.Close();
		}
		if (ImGui::MenuItem("Delete"))
		{
			GameObject::Destroy(rightClickMenu.target);
			rightClickMenu.Close();
		}
		
		if (ImGui::IsWindowHovered()) isHovered = true;

		if (!isHovered && ImGui::IsKeyPressed(ImGuiKey_MouseLeft)) { rightClickMenu.Close(); }
		
		ImGui::End();
	}
	else if (ImGui::IsWindowHovered())
	{
		if (!input->enabled && (ImGui::IsKeyPressed(ImGuiKey_Delete) || (ImGui::IsKeyPressed(ImGuiKey_KeypadDecimal))))
		{
			GameObject::Destroy(inspector->GetTarget());
		}
	}
}

void Heirarchy::DrawEntry(GameObject3D* gameObject3D)
{
	ImGui::PushID(GUI::GenerateID(gameObject3D).c_str());

	bool isSelected = inspector->GetTarget() == gameObject3D;
	if (ImGui::Selectable(gameObject3D->GetName(), &isSelected))
	{
		inspector->SetTarget(gameObject3D);
	}
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenOverlapped))
	{
		if (!input->enabled && ImGui::IsKeyPressed(ImGuiKey_MouseRight))
		{
			inspector->SetTarget(gameObject3D);
			rightClickMenu.Open(gameObject3D, ImGui::GetCursorScreenPos());
		}
	}
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
	{
		ImGui::SetDragDropPayload("GameObject3D", &gameObject3D, sizeof(GameObject3D**));
		if (ImGui::IsDragDropPayloadBeingAccepted())
		{
			std::stringstream tooltip;
			tooltip << "Move " << gameObject3D->GetClassName() << " here?";

			ImGui::Text(tooltip.str().c_str());
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
			GameObject3D** payloadObjectPtr = (GameObject3D**)payload->Data;
			GameObject3D* payloadObject = *payloadObjectPtr;

			if (payloadObject != target && !payloadObject->ContainsChild(target, true))
			{
				if (offset == ChildTo || offset == ChildToFront)
				{
					ImGui::AcceptDragDropPayload("GameObject3D");

					if (ImGui::IsKeyReleased(ImGuiKey_MouseLeft))
					{
						payloadObject->SetParent(target);

						if (offset == ChildToFront)
						{
							target->MoveChildTo(payloadObject, 0);
						}

						payloadObject->SetDirty();
					}
				}
				else
				{
					ImGui::AcceptDragDropPayload("GameObject3D");

					if (ImGui::IsKeyReleased(ImGuiKey_MouseLeft))
					{
						if (payloadObject->GetIndex() < target->GetIndex()) offset = (DragDropTargetOffset)(offset - 1);

						payloadObject->SetParent(target->GetParent());
						payloadObject->MoveTo(target->GetIndex() + offset);

						payloadObject->SetDirty();
					}
				}
			}
		}
		ImGui::EndDragDropTarget();
	}
}
