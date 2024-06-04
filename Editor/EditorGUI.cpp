#include "EditorGUI.h"

#include "ConsoleGUI.h"
#include "Heirarchy.h"
#include "SceneGUI.h"
#include "Inspector.h"
#include "ShaderProgramGUI.h"
#include "MaterialGUI.h"
#include "MeshCooker.h"
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
	unsigned int sceneViewFrameBufferHDR = 0;
	unsigned int sceneViewColourBufferHDR[2] = { 0, 0 };
	unsigned int sceneViewColourBufferGizmos = 0;
	unsigned int sceneViewDepthStencilBuffer = 0;
	unsigned int sceneViewFrameBufferOutput = 0;
	unsigned int sceneViewColourBufferOutput = 0;

	vector<EditorWindow> editorWindows;
	map<string, uint> windowIndices;

	void EditorGUI::Initialise() noexcept
	{
		// Window
		GLFWwindow* window = glfwGetCurrentContext();
		{
			glfwSetWindowTitle(window, "Editor");

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
			EditorWindow("Mesh Cooker", MeshCooker::Draw),
			EditorWindow("Camera", EditorCameraGUI::DrawEditorCameraGUI),
			EditorWindow("Editor Styler", EditorStyler::Draw),
			EditorWindow("Post Processing", PostProcessingGUI::Draw),
		};
		map<string, uint> windowIndices;

		ifstream editorSettingsFile("UserSettings\\EditorSettings.json");

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
			ImGui::IsKeyPressed(ImGuiKey_LeftCtrl) &&
			ImGui::IsKeyPressed(ImGuiKey_LeftShift);
		if (!isForceCloseComboPressed && !SceneGUI::ClearScene()) return false;

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
					if (ImGui::MenuItem(" Mesh Cooker", (const char*)0, false, !GetWindow("Mesh Cooker").open))
					{
						GetWindow("Mesh Cooker").open = true;
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
					if (ImGui::MenuItem(" GameObject3D")) { GameObject::Instantiate<GameObject3D>(); }
					if (ImGui::MenuItem(" MeshRenderer")) { GameObject::Instantiate<MeshRenderer>(); }
					if (ImGui::MenuItem(" LightObject")) { GameObject::Instantiate<LightObject>(); }
					if (ImGui::MenuItem(" ParticleEmitter")) { GameObject::Instantiate<ParticleEmitter>(); }
					ImGui::EndMenu();
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
				MeshCooker::fileQueue.push_back(files[i]);
			}
		}
	}
}
