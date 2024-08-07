#include "Heirarchy.h"

#include "GameObjectManager.h"
//#include "GameObject2D.h"
#include "GameObject3D.h"
#include "Camera.h"
#include "MeshRenderer.h"
#include "LightObject.h"
#include "ParticleEmitter.h"

#include "Inspector.h"

#include "Input.h"

#include "AppInfo.h"

namespace Heirarchy
{
	const float dragDropLineWidth = 2.5f;

	RightClickMenu rightClickMenu;

	void RightClickMenu::Open(GameObject* targetInit, ImVec2 positionInit)
	{
		open = true;
		position = positionInit;
		target = targetInit;
	}
	void RightClickMenu::Close()
	{
		open = false;
		target = nullptr;
	}

	void Heirarchy::Draw(const char* const name, bool& open)
	{
		ImGui::Begin(name, &open);

		ImGui::BeginDisabled();
		if (ImGui::CollapsingHeader("2D"))
		{

		}
		ImGui::EndDisabled();

		if (ImGui::CollapsingHeader("3D", ImGuiTreeNodeFlags_DefaultOpen))
		{
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
					ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, dragDropLineWidth);
					DragDropTarget("GameObject3D", (GameObject3D*)gameObjectManager->gameObjects[i], After);
					break;
				}
			}
			ImGui::Spacing();
		}

		if (rightClickMenu.open)
		{
			ImGui::SetNextWindowSize(ImVec2(160, 100));
			ImGui::SetNextWindowPos(rightClickMenu.position);
			ImGui::SetNextWindowBgAlpha(0.9f);

			ImGui::Begin(rightClickMenu.target->GetName(), (bool*)0, ImGuiWindowFlags_NoMove || ImGuiWindowFlags_NoDecoration);

			bool isHovered = false;

			if (ImGui::BeginMenu("Add New"))
			{
				if (ImGui::IsWindowHovered()) isHovered = true;

				if (dynamic_cast<GameObject3D*>(rightClickMenu.target) != nullptr)
				{
					GameObject3D* target = (GameObject3D*)rightClickMenu.target;

					if (ImGui::MenuItem("Prefab"))
					{
						using namespace FileDialogue;
						string filepath = GetLoadPath(PathDetails("Open Prefab", "Assets\\Prefabs\\", { "*.prefab" }), LimitToAssetFolder::True);

						if (filepath.size() > 0)
						{
							ifstream input(filepath);
							json prefab;

							bool parseError = false;
							try { input >> prefab; }
							catch (parse_error)
							{
								Debug::LogWarning(LogID::WRN102, filepath, locationinfo);
								parseError = true;
							}

							if (!parseError)
							{
								GameObject* instance = GameObject::InstantiateFrom(prefab, GuidGeneration::New);
								if (dynamic_cast<GameObject3D*>(instance) != nullptr)
								{
									gameObjectManager->Add(instance);
									((GameObject3D*)instance)->SetParent((GameObject3D*)rightClickMenu.target);
								}
								else
								{
									GameObject::Destroy(instance);
								}
							}
						}
						
						rightClickMenu.Close();
					}
					if (ImGui::MenuItem(GameObject3D::className))
					{
						GameObject3D::Instantiate<GameObject3D>(target);
						rightClickMenu.Close();
					}
					if (ImGui::MenuItem(MeshRenderer::className))
					{
						GameObject3D::Instantiate<MeshRenderer>(target);
						rightClickMenu.Close();
					}
					if (ImGui::MenuItem(LightObject::className))
					{
						GameObject3D::Instantiate<LightObject>(target);
						rightClickMenu.Close();
					}
					if (ImGui::MenuItem(ParticleEmitter::className))
					{
						GameObject3D::Instantiate<ParticleEmitter>(target);
						rightClickMenu.Close();
					}
				}
				ImGui::EndMenu();
			}

			ImGui::BeginDisabled(rightClickMenu.target->GetClassID() == Camera::classID);
			if (ImGui::MenuItem("Duplicate"))
			{
				Duplicate((GameObject3D*)rightClickMenu.target);

				rightClickMenu.Close();
			}
			if (ImGui::MenuItem("Save As Prefab"))
			{
				using namespace FileDialogue;
				string defaultPath = StringBuilder(
					"Assets\\Prefabs\\",
					rightClickMenu.target->GetName()
				).value;

				string filepath = GetSavePath(PathDetails("Save As Prefab", defaultPath.c_str(), {"*.prefab"}), LimitToAssetFolder::False);

				if (filepath.size() != 0)
				{
					json prefab = rightClickMenu.target;
					ofstream file(filepath);
					file << std::setw(2) << prefab;
				}
				
				rightClickMenu.Close();
			}
			if (ImGui::MenuItem("Delete"))
			{
				GameObject::Destroy(rightClickMenu.target);
				rightClickMenu.Close();
			}
			ImGui::EndDisabled();

			if (ImGui::IsWindowHovered()) isHovered = true;

			if (!isHovered && ImGui::IsKeyPressed(ImGuiKey_MouseLeft, false)) { rightClickMenu.Close(); }

			ImGui::End();
		}
		else if (ImGui::IsWindowHovered())
		{
			if (!AppInfo::input->enabled)
			{
				if (ImGui::IsKeyPressed(ImGuiKey_Delete, false) || ImGui::IsKeyPressed(ImGuiKey_KeypadDecimal, false))
				{
					GameObject::Destroy(inspector->GetTarget());
				}

				if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl))
				{
					if (ImGui::IsKeyPressed(ImGuiKey_D, false) && dynamic_cast<GameObject3D*>(rightClickMenu.target) != nullptr)
					{
						Duplicate((GameObject3D*)inspector->GetTarget());
					}
				}
			}
		}

		ImGui::End();
	}

	void Heirarchy::DrawEntry(GameObject3D* gameObject3D)
	{
		ImGui::PushID(GUI::GenerateID(gameObject3D).c_str());

		if (gameObject3D->GetIndex() == 0)
		{
			ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, dragDropLineWidth);
			DragDropTarget("GameObject3D", gameObject3D, Before);
		}

		bool isSelected = inspector->GetTarget() == gameObject3D;
		bool isLeaf = gameObject3D->GetChildren()->size() == 0;

		int flags = 0;
		flags |= ImGuiTreeNodeFlags_DefaultOpen;
		flags |= ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
		flags |= ImGuiTreeNodeFlags_FramePadding;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		flags |= ImGuiTreeNodeFlags_Selected * isSelected;
		flags |= ImGuiTreeNodeFlags_Leaf * isLeaf;

		if (ImGui::TreeNodeEx(gameObject3D->GetName(), flags))
		{
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenOverlapped))
			{
				AcceptInputFor(gameObject3D);
			}
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
			{
				BeginPayload(gameObject3D);
				ImGui::EndDragDropSource();
			}
			DragDropTarget("GameObject3D", gameObject3D);

			for (int i = 0; i < gameObject3D->GetChildren()->size(); i++)
			{
				DrawEntry((*gameObject3D->GetChildren())[i]);
			}
			ImGui::TreePop();
		}
		else
		{
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenOverlapped))
			{
				AcceptInputFor(gameObject3D);
			}
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
			{
				BeginPayload(gameObject3D);
				ImGui::EndDragDropSource();
			}
			DragDropTarget("GameObject3D", gameObject3D);
		}

		if (isLeaf)
		{
			ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, dragDropLineWidth);
			DragDropTarget("GameObject3D", gameObject3D, After);
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
							payloadObject->SetParent(target->GetParent());
							if (payloadObject->GetIndex() < target->GetIndex()) offset = (DragDropTargetOffset)(offset - 1);
							payloadObject->MoveTo(target->GetIndex() + offset);

							payloadObject->SetDirty();
						}
					}
				}
			}
			ImGui::EndDragDropTarget();
		}
	}

	void Heirarchy::AcceptInputFor(GameObject3D* gameObject3D)
	{
		if (!AppInfo::input->enabled && ImGui::IsKeyReleased(ImGuiKey_MouseLeft))
		{
			inspector->SetTarget(gameObject3D);
		}
		if (!AppInfo::input->enabled && ImGui::IsKeyReleased(ImGuiKey_MouseRight))
		{
			inspector->SetTarget(gameObject3D);
			rightClickMenu.Open(gameObject3D, ImGui::GetCursorScreenPos());
		}
	}
}

void Heirarchy::BeginPayload(GameObject3D* gameObject3D)
{
	ImGui::SetDragDropPayload("GameObject3D", &gameObject3D, sizeof(GameObject3D**));
	if (ImGui::IsDragDropPayloadBeingAccepted())
	{
		std::stringstream tooltip;
		tooltip << "Move " << gameObject3D->GetName() << " here?";

		ImGui::Text(tooltip.str().c_str());
	}
	else
	{
		ImGui::TextColored({ 255, 0, 0, 1 }, "Invalid target");
	}
}

void Heirarchy::Duplicate(GameObject3D* gameObject3D) noexcept
{
	GameObject3D* parent = gameObject3D->GetParent();

	json prefab = rightClickMenu.target;

	GameObject3D* clone = (GameObject3D*)GameObject::InstantiateFrom(prefab, GuidGeneration::New);
	gameObjectManager->Add(clone);
	clone->SetParent(parent);
	clone->MoveTo(gameObject3D->GetIndex() + 1);
}
