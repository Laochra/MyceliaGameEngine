#include "SceneGUI.h"

#include "Scene.h"

#include "EditorGUI.h"
#include "EditorCamera.h"

#include "JsonIncludes.h"
#include "Inspector.h"
#include "GameObjectManager.h"
#include "GameObject.h"

#include "GeneralMacros.h"

#include "Debug.h"

namespace SceneGUI
{
	void SceneGUI::DrawScene(const char* const name, bool& open) noexcept
	{
		GLFWwindow* window = glfwGetCurrentContext();

		ImVec2 oldPadding = ImGui::GetStyle().WindowPadding;
		ImGui::GetStyle().WindowPadding = ImVec2(0.0f, 0.0f);

		ImGui::Begin(name, &open);

		screenWidth = (int)ImGui::GetWindowWidth();
		float titleBarHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2;
		screenHeight = (int)(ImGui::GetWindowHeight() - titleBarHeight);

		if (ImGui::IsWindowHovered())
		{
			if (EditorCamera::main()->freeCamera.pressed())
			{
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				if (glfwRawMouseMotionSupported()) glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
				ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse; // Disables Imgui's Mouse Input
			}
			input->enabled = true;
		}
		else if (input->enabled && !EditorCamera::main()->freeCamera.down())
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse; // Re-enables Imgui's Mouse Input
			input->enabled = false;
		}

		if (EditorCamera::main()->freeCamera.released())
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse; // Re-enables Imgui's Mouse Input
			input->enabled = false;
		}

		uintptr_t image = (uintptr_t)EditorGUI::sceneViewColourBufferOutput; // Casting to a uintptr_t is required to stop a warning with converting 32bit uint to 64bit void*. ImGui::Image works regardless.
		ImGui::Image((void*)image, ImVec2((float)screenWidth, (float)screenHeight), ImVec2(0, 1), ImVec2(1, 0));
		
		ImGui::End();

		ImGui::GetStyle().WindowPadding = oldPadding;
	}

	void SceneGUI::DrawFileDropdown() noexcept
	{
		if (ImGui::MenuItem(" New Scene", "Ctrl+N"))
		{
			NewScene();
		}
		if (ImGui::MenuItem(" Open Scene", "Ctrl+O"))
		{
			OpenScene();
		}
		if (ImGui::MenuItem(" Save Scene", "Ctrl+S"))
		{
			SaveScene();
		}
		if (ImGui::MenuItem(" Save Scene As", "Ctrl+Shift+S"))
		{
			SaveSceneAs();
		}
	}

	bool SceneGUI::EnsureClearIsIntentional() noexcept
	{
		const char* const dialogueTitle = "Unsaved scene data will be lost!";
		const char* const dialogueMessage = "Would you like to save your changes?";
		const char* const dialogueType = "yesnocancel";
		const char* const dialogueIcon = "warning";

		using namespace FileDialogue;
		MessageBox messageBox(
			"Unsaved scene data will be lost!",
			"Would you like to save your changes?",
			MBType::YesNoCancel, MBIcon::Warning
		);
		int selectedOption = DisplayMessageBox(messageBox);

		switch (selectedOption)
		{
		default: Debug::LogWarning("Unexpected return value from save message box", locationinfo); return false;
		case 0: return false;			// int 0 means Cancel
		case 1: SaveScene(); break;	// int 1 means Yes, do save changes
		case 2: break;						// int 2 means No, don't save changes
		}

		inspector->SetTarget(nullptr);
		Scene::Clear();
		return true;
	}
	
	bool NewScene() noexcept
	{
		if (!EnsureClearIsIntentional()) return false;
		Scene::Clear();
		Scene::currentPath = nullptr;
		Scene::dirty = false;
		return true;
	}
	bool OpenScene() noexcept
	{
		using namespace FileDialogue;
		string scenePath = GetLoadPath(PathDetails("Open Scene", "Assets\\Scenes\\", { "*.scene" }), LimitToAssetFolder::True);
		if (scenePath.size() == 0) return false;

		if (!EnsureClearIsIntentional()) return false;

		return Scene::Open(scenePath.c_str());
	}

	bool SaveScene() noexcept
	{
		if (Scene::currentPath == nullptr) return SaveSceneAs();
		else return Scene::Save();
	}

	bool SaveSceneAs() noexcept
	{
		using namespace FileDialogue;
		string filepath = GetSavePath(PathDetails("Save Scene As", "Assets\\Scenes\\NewScene", { "*.scene" }), LimitToAssetFolder::False);

		if (filepath.size() == 0) return false;

		Scene::currentPath = filepath.c_str();

		return Scene::Save();
	}
}
