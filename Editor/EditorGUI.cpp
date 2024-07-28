#include "EditorGUI.h"

#include "ConsoleGUI.h"
#include "Heirarchy.h"
#include "SceneGUI.h"
#include "Inspector.h"
#include "ShaderProgramGUI.h"
#include "MaterialGUI.h"
#include "ArtImporter.h"
#include "EditorStyler.h"
#include "PostProcessingGUI.h"

#include "MeshRendererGUI.h"
#include "EditorCameraGUI.h"
#include "LightObjectGUI.h"
#include "ParticleEmitter.h"

#include "stb/stb_image.h"

#include <algorithm>

namespace EditorGUI
{
	// Scene Buffers
	unsigned int sceneViewTexture = 0;

	unsigned int guidFBO = 0;
	unsigned int guidDepth = 0;
	unsigned int guidTexture = 0;

	vector<EditorWindow> editorWindows;
	map<string, uint> windowIndices;

	void EditorGUI::Initialise() noexcept
	{
		// Window
		GLFWwindow* window = glfwGetCurrentContext();
		{
			GLFWimage icons[4]{};
			icons[0].pixels = stbi_load("Engine\\Icons\\shroom16x16.png", &icons[0].width, &icons[0].height, 0, 4);
			icons[1].pixels = stbi_load("Engine\\Icons\\shroom32x32.png", &icons[1].width, &icons[1].height, 0, 4);
			icons[2].pixels = stbi_load("Engine\\Icons\\shroom48x48.png", &icons[2].width, &icons[2].height, 0, 4);
			icons[3].pixels = stbi_load("Engine\\Icons\\shroom64x64.png", &icons[3].width, &icons[3].height, 0, 4);
			glfwSetWindowIcon(window, 4, icons);
			stbi_image_free(icons[0].pixels);
			stbi_image_free(icons[1].pixels);
			stbi_image_free(icons[2].pixels);
			stbi_image_free(icons[3].pixels);
		}

		// ImGui
		{
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			//ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;	// Enable Keyboard Controls
			//ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;	// Enable Gamepad Controls
			ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;			// Enable Docking
			ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;		// Enable Viewports
			ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;

			ImGui_ImplGlfw_InitForOpenGL(window, true);
			ImGui_ImplOpenGL3_Init();

			ImGui::GetIO().IniFilename = "Engine\\GUIState.ini";
		}

		// Inspector
		inspector = new Inspector();
		// ShaderProgramGUI
		ShaderProgramGUI::ValidateLinkage();
		// MaterialGUI
		MaterialGUI::LoadShaderProgram("PBRLit");
		MaterialGUI::dirty = false;

		editorWindows =
		{
			EditorWindow("Console", ConsoleGUI::Draw),
			EditorWindow("Heirarchy", Heirarchy::Draw),
			EditorWindow("Inspector", Inspector::Draw),
			EditorWindow("Scene", SceneGUI::DrawScene),
			EditorWindow("Shader Program Editor", ShaderProgramGUI::Draw),
			EditorWindow("Material Editor", MaterialGUI::Draw),
			EditorWindow("Art Importer", ArtImporter::Draw),
			EditorWindow("Camera", EditorCameraGUI::DrawEditorCameraGUI),
			EditorWindow("Editor Styler", EditorStyler::Draw),
			EditorWindow("Post Processing", PostProcessingGUI::Draw),
		};
		map<string, uint> windowIndices;

		const char* editorSettingsFilepath = "UserSettings\\EditorSettings.json";
		ifstream editorSettingsFile(editorSettingsFilepath);

		if (!editorSettingsFile.good())
		{
			ofstream defaults(editorSettingsFilepath);
			defaults <<
				"{\n"
				"  \"OpenWindows\": [\n"
				"    \"Console\",\n"
				"    \"Heirarchy\",\n"
				"    \"Inspector\",\n"
				"    \"Scene\"\n"
				"  ]\n"
				"}\n";
			editorSettingsFile = ifstream(editorSettingsFilepath);
		}

		json editorSettings;
		editorSettingsFile >> editorSettings;
		vector<string> windows = editorSettings["OpenWindows"];
		
		for (int i = 0; i < editorWindows.size(); i++)
		{
			editorWindows[i].open = std::find(windows.begin(), windows.end(), editorWindows[i].name) != windows.end();
		}

		// Styling
		{
			ImGuiStyle* style = &ImGui::GetStyle();

			style->ItemInnerSpacing = { 3.0f, 3.0f };
			style->ItemSpacing = { 3.0f, 3.0f };

			style->TabRounding = 4.0f;
			style->TabBarBorderSize = 2.0f;

			style->FrameRounding = 4.0f;
			style->FramePadding = { 4.0f, 3.5f };

			style->PopupRounding = 4.0f;

			style->WindowRounding = 4.0f;
			style->WindowPadding = { 10.0f, 10.0f };

			style->SelectableRounding = 5.0f;

			style->ScrollbarRounding = 4.0f;
			style->ScrollbarSize = 16.0f;

			GUI::LoadStyle(GUI::currentStyle);
		}
	}

	bool EditorGUI::CleanUp() noexcept
	{
		bool isForceCloseComboPressed =
			ImGui::IsKeyDown(ImGuiKey_LeftCtrl) &&
			ImGui::IsKeyDown(ImGuiKey_LeftShift);
		if (!isForceCloseComboPressed && !SceneGUI::EnsureClearIsIntentional()) return false;

		json editorSettings;
		vector<string> windows;

		for (int i = 0; i < editorWindows.size(); i++)
		{
			if (editorWindows[i].open) windows.push_back(editorWindows[i].name);
		}

		editorSettings["OpenWindows"] = windows;

		ofstream output("UserSettings\\EditorSettings.json");
		output << std::setw(2) << editorSettings;

		del(inspector);

		return true;
	}

	void ReadyImGui() noexcept
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
	}
	void DrawMenuBar() noexcept
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				SceneGUI::DrawFileDropdown();
				ImGui::EndMenu();
			}
			SceneGUI::CheckForFileShortcuts();
			GUI::Spacing(4);
			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem(" Undo ", "Ctrl+Z", false, GUI::editHistory.CanUndo()))
				{
					GUI::editHistory.Undo();
				}
				if (ImGui::MenuItem(" Redo ", "Ctrl+Y", false, GUI::editHistory.CanRedo()))
				{
					GUI::editHistory.Redo();
				}
				ImGui::EndMenu();
			}
			if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
			{
				if (ImGui::IsKeyPressed(ImGuiKey_Z, false) && GUI::editHistory.CanUndo())
				{
					GUI::editHistory.Undo();
				}
				else if (ImGui::IsKeyPressed(ImGuiKey_Y, false) && GUI::editHistory.CanRedo())
				{
					GUI::editHistory.Redo();
				}
			}
			GUI::Spacing(4);
			if (ImGui::BeginMenu("Windows"))
			{
				if (ImGui::MenuItem(" Console", (const char*)0, false, !GetWindow("Console").open))
				{
					GetWindow("Console").open = true;
				}
				if (ImGui::MenuItem(" Heirarchy", (const char*)0, false, !GetWindow("Heirarchy").open))
				{
					GetWindow("Heirarchy").open = true;
				}
				if (ImGui::MenuItem(" Inspector", (const char*)0, false, !GetWindow("Inspector").open))
				{
					GetWindow("Inspector").open = true;
				}
				if (ImGui::MenuItem(" Scene View", (const char*)0, false, !GetWindow("Scene").open))
				{
					GetWindow("Scene").open = true;
				}

				if (ImGui::BeginMenu(" Settings"))
				{
					if (ImGui::MenuItem(" Camera", (const char*)0, false, !GetWindow("Camera").open))
					{
						GetWindow("Camera").open = true;
					}
					if (ImGui::MenuItem(" Post Processing", (const char*)0, false, !GetWindow("Post Processing").open))
					{
						GetWindow("Post Processing").open = true;
					}
					if (ImGui::MenuItem(" Editor Styler", (const char*)0, false, !GetWindow("Editor Styler").open))
					{
						GetWindow("Editor Styler").open = true;
					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu(" Tools"))
				{
					if (ImGui::MenuItem(" Shader Program Editor", (const char*)0, false, !GetWindow("Shader Program Editor").open))
					{
						GetWindow("Shader Program Editor").open = true;
					}
					if (ImGui::MenuItem(" Material Editor", (const char*)0, false, !GetWindow("Material Editor").open))
					{
						GetWindow("Material Editor").open = true;
					}
					if (ImGui::MenuItem(" Art Importer", (const char*)0, false, !GetWindow("Art Importer").open))
					{
						GetWindow("Art Importer").open = true;
					}
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
			GUI::Spacing(4);
			if (ImGui::BeginMenu("Create"))
			{
				if (ImGui::BeginMenu(" 2D   ", false))
				{
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu(" 3D   "))
				{
					if (ImGui::MenuItem(GameObject3D::className)) { GameObject3D::Instantiate<GameObject3D>(glm::identity<mat4>()); }
					if (ImGui::MenuItem(MeshRenderer::className)) { GameObject3D::Instantiate<MeshRenderer>(glm::identity<mat4>()); }
					if (ImGui::MenuItem(LightObject::className)) { GameObject3D::Instantiate<LightObject>(glm::identity<mat4>()); }
					if (ImGui::MenuItem(ParticleEmitter::className)) { GameObject3D::Instantiate<ParticleEmitter>(glm::identity<mat4>()); }
					ImGui::EndMenu();
				}
				if (ImGui::MenuItem(" Prefab"))
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
							Debug::LogWarning(LogID::WRN102, filepath, locationinfo );
							parseError = true;
						}

						if (!parseError)
						{
							gameObjectManager->Add(GameObject::InstantiateFrom(prefab, GuidGeneration::New));
						}
					}
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	}

	void EditorGUI::Draw() noexcept
	{
		ReadyImGui();

		DrawMenuBar();

		for (int i = 0; i < editorWindows.size(); i++)
		{
			editorWindows[i].Draw();
		}
	}

	void EditorGUI::RenderGUI() noexcept
	{
		GLFWwindow* window = glfwGetCurrentContext();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glEnable(GL_DEPTH_TEST);

		int width, height;
		glfwGetWindowSize(window, &width, &height);

		glViewport(0, 0, width, height);

		// Draw ImGui UI
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(window);
		}
	}

	EditorWindow& EditorGUI::GetWindow(string name) noexcept
	{
		return editorWindows[windowIndices[name]];
	}

	static bool CheckIfStringContains(string& str, StringParams tests)
	{
		for (string test : tests)
		{
			if (str.find(test) != string::npos)
			{
				return true;
			}
		}
		return false;
	}

	void EditorGUI::FileDropCallback(GLFWwindow* window, int count, const char** filepaths) noexcept
	{
		vector<string> files;

		for (int i = 0; i < count; i++)
		{
			files.push_back(filepaths[i]);
		}

		for (int i = 0; i < files.size(); i++)
		{
			if (CheckIfStringContains(files[i], { ".fbx", ".obj", ".gltf" }))
			{
				ArtImporter::Add(files[i]);
			}
		}
	}
}
