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

		int selectedOption = tinyfd_messageBox(dialogueTitle, dialogueMessage, dialogueType, dialogueIcon, 0);

		switch (selectedOption)
		{
		default: Debug::Log(Debug::WRN("Unexpected return value from save message box")); return false;
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
		const char* const windowTitle = "Open Scene";
		const uint defaultPathLength = 15;
		const char defaultPath[defaultPathLength] = "Assets\\Scenes\\";
		const uint filterPatternCount = 1;
		const char* const filterPatterns[filterPatternCount] = { "*.scene" };

		const char* const filePath = tinyfd_openFileDialog(windowTitle, defaultPath, filterPatternCount, filterPatterns, nullptr, false);
		if (filePath == nullptr) return false;
		const uint filePathLength = (uint)strlen(filePath);

		const uint startOffset = (uint)string(filePath).find("Assets\\");

		if (startOffset == string::npos)
		{
			Debug::Log
			(
				Debug::WRN("Assets should only be loaded from inside the Assets folder. "
				"Assets outside this folder won't be included in builds. "
				"Scene was not saved!"),
				Debug::WRN105
			);
			return false;
		}

		if (!EnsureClearIsIntentional()) return false;

		return Scene::Open(filePath + startOffset);
	}

	bool SaveScene() noexcept
	{
		if (Scene::currentPath == nullptr) return SaveSceneAs();
		else return Scene::Save();
	}

	bool SaveSceneAs() noexcept
	{
		const char* const windowTitle = "Save Scene As";
		const uint defaultPathLength = 23;
		const char defaultPath[defaultPathLength] = "Assets\\Scenes\\NewScene";
		const uint filterPatternCount = 1;
		const char* const filterPatterns[filterPatternCount] = { "*.scene" };

		const char* const filePath = tinyfd_saveFileDialog(windowTitle, defaultPath, filterPatternCount, filterPatterns, nullptr);
		if (filePath == nullptr) return false;
		const uint filePathLength = (uint)strlen(filePath);

		const uint startOffset = (uint)string(filePath).find("Assets\\");

		if (startOffset == string::npos)
		{
			Debug::Log
			(
				Debug::WRN("Assets should only be saved inside the Assets folder. "
					"Assets outside this folder won't be included in builds. "
					"Scene was not saved! "
					locationinfo),
				Debug::WRN121
			);
			return false;
		}

		Scene::currentPath = filePath + startOffset;

		return Scene::Save();
	}
}
