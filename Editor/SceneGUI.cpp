#include "SceneGUI.h"

#include "Scene.h"

#include "EditorGUI.h"
#include "EditorCamera.h"

#include "JsonIncludes.h"
#include "Inspector.h"
#include "GameObjectManager.h"
#include "GameObject.h"

#include "GeneralMacros.h"

#include "TransformEdit.h"
#include "Debug.h"
#include <iostream>

#include "AppInfo.h"

typedef unsigned long long ulong;

namespace SceneGUI
{
	vec2 normalisedMousePos = vec2(0.0f, 0.0f);

	void SceneGUI::DrawScene(const char* const name, bool& open) noexcept
	{
		GLFWwindow* window = glfwGetCurrentContext();

		ImVec2 oldPadding = ImGui::GetStyle().WindowPadding;
		ImGui::GetStyle().WindowPadding = ImVec2(0.0f, 0.0f);

		ImGui::Begin(name, &open, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar );

		if (ImGui::BeginMenuBar())
		{
			ImVec4 shortcutColour = ImGui::GetStyle().Colors[ImGuiCol_TextDisabled];

			GUI::Spacing(2);

			ImGui::PushItemWidth(ImGui::GetWindowWidth() / 10);
			const char* modeName;
			switch (TransformEdit::mode)
			{
			default:											modeName = "Select";		break;
			case TransformEdit::Mode::Translate:	modeName = "Translate"; break;
			case TransformEdit::Mode::Rotate:		modeName = "Rotate";		break;
			}
			if (ImGui::BeginCombo("##Mode", modeName))
			{
				ImGui::Spacing();
				if (ImGui::Selectable(" Select    ", TransformEdit::mode == TransformEdit::Mode::Select))
				{
					TransformEdit::mode = TransformEdit::Mode::Select;
				}
				ImGui::SameLine();
				ImGui::TextColored(shortcutColour, "Q");
				ImGui::Spacing();
				if (ImGui::Selectable(" Translate ", TransformEdit::mode == TransformEdit::Mode::Translate))
				{
					TransformEdit::mode = TransformEdit::Mode::Translate;
				}
				ImGui::SameLine();
				ImGui::TextColored(shortcutColour, "W");
				ImGui::Spacing();
				if (ImGui::Selectable(" Rotate    ", TransformEdit::mode == TransformEdit::Mode::Rotate))
				{
					TransformEdit::mode = TransformEdit::Mode::Rotate;
				}
				ImGui::SameLine();
				ImGui::TextColored(shortcutColour, "E");
				ImGui::Spacing();
				ImGui::EndCombo();
			}

			GUI::Spacing(3);

			const char* spaceName;
			switch (TransformEdit::space)
			{
			default:										spaceName = "Global";	break;
			case TransformEdit::Space::Local:	spaceName = "Local";	break;
			}
			if (ImGui::BeginCombo("##Space", spaceName))
			{
				ImGui::Spacing();
				if (ImGui::Selectable(" Global    ", TransformEdit::space == TransformEdit::Space::Global))
				{
					TransformEdit::space = TransformEdit::Space::Global;
				}
				ImGui::SameLine();
				ImGui::TextColored(shortcutColour, "T");
				ImGui::Spacing();
				if (ImGui::Selectable(" Local     ", TransformEdit::space == TransformEdit::Space::Local))
				{
					TransformEdit::space = TransformEdit::Space::Local;
				}
				ImGui::SameLine();
				ImGui::TextColored(shortcutColour, "T");
				ImGui::Spacing();
				ImGui::EndCombo();
			}
			ImGui::PopItemWidth();

			GUI::Spacing(3);

			ImGui::BeginDisabled(AppInfo::state == AppState::Playing);
			if (ImGui::Button("Play"))
			{
				AppInfo::state = AppState::Playing;
			}
			ImGui::EndDisabled();
			ImGui::BeginDisabled(AppInfo::state == AppState::Editor);
			if (ImGui::Button("Stop"))
			{
				AppInfo::state = AppState::Editor;
			}
			ImGui::EndDisabled();
			ImGui::BeginDisabled(AppInfo::state != AppState::Playing);
			if (ImGui::Button("Pause"))
			{
				AppInfo::state = AppState::Paused;
			}
			ImGui::EndDisabled();

			GUI::Spacing(3);

			ImGui::Checkbox("Game UI", &EditorGUI::displayUI);

			ImGui::EndMenuBar();
		}
		ImVec2 menuBarSize = ImGui::GetItemRectSize();

		float titleBarHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2;
		glm::ivec2 newScreenSize((int)ImGui::GetWindowWidth(), (int)(ImGui::GetWindowHeight() - (titleBarHeight + menuBarSize.y)));
		if (AppInfo::screenWidth != newScreenSize.x || AppInfo::screenHeight != newScreenSize.y)
		{
			AppInfo::screenWidth = newScreenSize.x;
			AppInfo::screenHeight = newScreenSize.y;
			AppInfo::screenSizeJustChanged = true;
		}
		else
		{
			AppInfo::screenSizeJustChanged = false;
		}

		if (AppInfo::screenWidth <= 0 || AppInfo::screenHeight <= 0)
		{
			ImGui::End();
			ImGui::GetStyle().WindowPadding = oldPadding;
			return;
		}

		if (AppInfo::input->GetKeyPressed(KeyCode::Esc))
		{
			TransformEdit::CancelTransform();
		}

		if (((EditorCamera*)AppInfo::editorCamera)->freeCamera.released())
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse; // Re-enables Imgui's Mouse Input
			AppInfo::input->enabled = false;
		}

		uintptr_t image = (uintptr_t)EditorGUI::sceneViewTexture; // Casting to a uintptr_t is required to stop a warning with converting 32bit uint to 64bit void*. ImGui::Image works regardless.
		ImGui::Image((ImTextureID)image, ImVec2((float)AppInfo::screenWidth, (float)AppInfo::screenHeight), ImVec2(0, 1), ImVec2(1, 0));
		
		ImVec2 imguiWindowPos = ImGui::GetWindowPos();
		int mainWindowX, mainWindowY;
		glfwGetWindowPos(window, &mainWindowX, &mainWindowY);
		vec2& cursorPos = AppInfo::input->screenCursorPos;
		cursorPos = AppInfo::input->cursorPos - vec2(imguiWindowPos.x, imguiWindowPos.y + menuBarSize.y + 23) + vec2(mainWindowX, mainWindowY);
		cursorPos.y = AppInfo::screenHeight - cursorPos.y;
		normalisedMousePos = vec2(cursorPos.x / AppInfo::screenWidth, cursorPos.y / AppInfo::screenHeight);
		normalisedMousePos = (normalisedMousePos * 2.0f) - 1.0f;

		if (AppInfo::state == AppState::Editor)
		{
			if (ImGui::IsItemHovered())
			{
				AppInfo::input->enabled = true;

				if (((EditorCamera*)AppInfo::editorCamera)->freeCamera.pressed())
				{
					TransformEdit::CancelTransform();

					glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
					if (glfwRawMouseMotionSupported()) glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
					ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse; // Disables Imgui's Mouse Input
				}
				else
				{
					if (AppInfo::input->GetKeyPressed(KeyCode::Q))
					{
						TransformEdit::CancelTransform();
						TransformEdit::mode = TransformEdit::Mode::Select;
					}
					if (AppInfo::input->GetKeyPressed(KeyCode::W))
					{
						TransformEdit::CancelTransform();
						TransformEdit::mode = TransformEdit::Mode::Translate;
					}
					if (AppInfo::input->GetKeyPressed(KeyCode::E))
					{
						TransformEdit::CancelTransform();
						TransformEdit::mode = TransformEdit::Mode::Rotate;
					}
					if (AppInfo::input->GetKeyPressed(KeyCode::T))
					{
						TransformEdit::CancelTransform();
						TransformEdit::space = (TransformEdit::Space)!(bool)TransformEdit::space;
					}
				}

				if (AppInfo::input->GetKeyPressed(KeyCode::MouseLeft))
				{
					const int pixelCount = AppInfo::screenWidth * AppInfo::screenHeight;
					ulong* guidPixels = new ulong[pixelCount];
					glGetTextureImage(EditorGUI::guidTexture, 0, GL_RG_INTEGER, GL_UNSIGNED_INT, pixelCount * sizeof(ulong), guidPixels);

					if (cursorPos.x > 0 && cursorPos.x < AppInfo::screenWidth && cursorPos.y > 0 && cursorPos.y < AppInfo::screenHeight)
					{
						uint guidIndex = (int)cursorPos.x + (int)cursorPos.y * AppInfo::screenWidth;
						ulong guid = guidPixels[guidIndex];
						switch (guid)
						{
						case (ulong)TransformEdit::Handle::X:
						case (ulong)TransformEdit::Handle::Y:
						case (ulong)TransformEdit::Handle::Z:
							vec2 normalisedMousePos = vec2(cursorPos.x / AppInfo::screenWidth, cursorPos.y / AppInfo::screenHeight);
							normalisedMousePos *= 2.0f;
							normalisedMousePos -= 1.0f;
							TransformEdit::BeginTransform((TransformEdit::Handle)guid, normalisedMousePos);
							break;
						default: inspector->SetTarget(gameObjectManager->Find(guid)); break;
						}
					}

					delete guidPixels;
				}

			}
			else if (AppInfo::input->enabled && !((EditorCamera*)AppInfo::editorCamera)->freeCamera.down())
			{
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse; // Re-enables Imgui's Mouse Input
				AppInfo::input->enabled = false;
			}
		}
		else if (AppInfo::state == AppState::Playing)
		{
			if (ImGui::IsItemHovered())
			{
				AppInfo::input->enabled = true;
			}
			else
			{
				AppInfo::input->enabled = false;
			}
		}

		ImGui::End();

		ImGui::GetStyle().WindowPadding = oldPadding;
	}

	void SceneGUI::DrawFileDropdown() noexcept
	{
		if (ImGui::MenuItem(" New Scene ", "Ctrl+N"))
		{
			NewScene();
		}
		if (ImGui::MenuItem(" Open Scene ", "Ctrl+O"))
		{
			OpenScene();
		}
		if (ImGui::MenuItem(" Save Scene ", "Ctrl+S"))
		{
			SaveScene();
		}
		if (ImGui::MenuItem(" Save Scene As ", "Ctrl+Shift+S"))
		{
			SaveSceneAs();
		}
	}
	void CheckForFileShortcuts() noexcept
	{
		if (AppInfo::input->enabled && ((EditorCamera*)AppInfo::editorCamera)->freeCamera.down()) return;

		if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl))
		{
			if (ImGui::IsKeyPressed(ImGuiKey_N, false)) NewScene();
			if (ImGui::IsKeyPressed(ImGuiKey_O, false)) OpenScene();
			if (ImGui::IsKeyPressed(ImGuiKey_S, false))
			{
				if (ImGui::IsKeyDown(ImGuiKey_LeftShift)) SaveSceneAs();
				else SaveScene();
			}
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
		Scene::NewScene();
		Scene::currentPath.clear();
		Scene::dirty = false;
		return true;
	}
	bool OpenScene() noexcept
	{
		using namespace FileDialogue;
		string scenePath = GetLoadPath(PathDetails("Open Scene", "Assets\\Scenes\\", { "*.scene" }), LimitToAssetFolder::True);
		if (scenePath.size() == 0) return false;

		if (!EnsureClearIsIntentional()) return false;

		bool openSuccess = Scene::Open(scenePath.c_str());
		return openSuccess;
	}

	bool SaveScene() noexcept
	{
		if (Scene::currentPath.size() == 0) return SaveSceneAs();
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
