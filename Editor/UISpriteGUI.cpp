#include "UISpriteGUI.h"

#include "GUI.h"

void UISpriteGUI::Draw()
{
	DrawUISpriteGUI((UISprite*)target);
}

void UISpriteGUI::DrawUISpriteGUI(UISprite* uiSprite)
{
	if (uiSprite == nullptr) return;

	std::string idStr = "UISprite###" + std::to_string(intptr_t(uiSprite));
	const char* id = idStr.c_str();

	if (ImGui::CollapsingHeader(id, ImGuiTreeNodeFlags_DefaultOpen))
	{
		std::string tempName(uiSprite->GetName());
		if (ImGui::InputText(" ", &tempName))
		{
			uiSprite->SetName(tempName.c_str());
		}
		ImGui::SameLine();
		ImGui::Checkbox("Enabled", &uiSprite->enabled);

		GUI::Spacing(3);

		if (ImGui::Button("Load"))
		{
			string loadPath = FileDialogue::GetLoadPath(FileDialogue::PathDetails("Load Sprite Texture", "Assets\\", { "*.png", "*.tga", "*.jpg" }), FileDialogue::LimitToAssetFolder::True);
			if (!loadPath.empty())
			{
				uiSprite->Load(loadPath.c_str());
			}
		}
		ImGui::SameLine();
		std::string spritePath = uiSprite->GetSpritePath();
		ImGui::InputText("Sprite", &spritePath, ImGuiInputTextFlags_ReadOnly);

		GUI::Spacing(3);

		ImGui::PushItemWidth(ImGui::CalcItemWidth() / 1.5f);
		ImGui::DragFloat2("Position", (float*)&uiSprite->normalisedPosition, 0.001f, 0.0f, 0.0f, "%.5f");
		ImGui::PopItemWidth();
		ImGui::PushItemWidth(ImGui::CalcItemWidth() / 3.025f);
		ImGui::DragFloat("Scale", &uiSprite->scale, 0.001f, 0.0f, 0.0f, "%.5f");
		ImGui::PopItemWidth();

		GUI::Spacing(3);
	}
}
