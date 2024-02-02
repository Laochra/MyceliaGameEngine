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
}

void Heirarchy::Draw()
{
	if (ImGui::CollapsingHeader("2D"))
	{

		ImGui::Spacing();
	}

	if (ImGui::CollapsingHeader("3D"))
	{
		for (GameObject3D* gameObject3D : gameObjectManager->gameObject3Ds)
		{
			DrawEntry(gameObject3D);
		}

		ImGui::Spacing();
	}
}

void Heirarchy::DrawEntry(GameObject3D* gameObject3D)
{
	ImGui::Text(std::to_string(gameObject3D->GetGUID()).c_str());

	if (gameObject3D->GetChildren()->size() != 0)
	{
		ImGui::Indent();
		for (GameObject3D* child : *gameObject3D->GetChildren())
		{
			DrawEntry(child);
		}
		ImGui::Unindent();
	}
}