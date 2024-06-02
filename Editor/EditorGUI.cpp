#include "EditorGUI.h"

#include "ShaderProgramGUI.h"
#include "MaterialGUI.h"
#include "ConsoleGUI.h"
#include "Heirarchy.h"
#include "SceneGUI.h"
#include "Inspector.h"
#include "EditorStyler.h"
#include "MeshCooker.h"

#include "MeshRendererGUI.h"
#include "EditorCameraGUI.h"
#include "LightObjectGUI.h"

#include "stb/stb_image.h"

extern float exposure; // TODO: This sucks. Move this once post-processing has its own place

namespace EditorGUI
{
	// Scene Buffers
	unsigned int sceneViewFrameBufferHDR = 0;
	unsigned int sceneViewColourBufferHDR[2] = { 0, 0 };
	unsigned int sceneViewColourBufferGizmos = 0;
	unsigned int sceneViewDepthStencilBuffer = 0;
	unsigned int sceneViewFrameBufferOutput = 0;
	unsigned int sceneViewColourBufferOutput = 0;

	// Editor Windows
	bool consoleOpen = true;
	bool heirarchyOpen = true;
	bool inspectorOpen = true;
	bool sceneViewOpen = true;
	bool shaderProgramEditorOpen = true;
	bool materialEditorOpen = true;
	bool meshCookerOpen = true;
	bool cameraSettingsOpen = true;
	bool editorStylerOpen = true;
	bool postProcessingOpen = true;

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
		if (!SceneGUI::ClearScene()) return false;

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
				if (ImGui::MenuItem(" Console", (const char*)0, false, !consoleOpen))
				{
					consoleOpen = true;
				}
				if (ImGui::MenuItem(" Heirarchy", (const char*)0, false, !heirarchyOpen))
				{
					heirarchyOpen = true;
				}
				if (ImGui::MenuItem(" Inspector", (const char*)0, false, !inspectorOpen))
				{
					inspectorOpen = true;
				}
				if (ImGui::MenuItem(" Scene View", (const char*)0, false, !sceneViewOpen))
				{
					sceneViewOpen = true;
				}

				if (ImGui::BeginMenu(" Settings"))
				{
					if (ImGui::MenuItem(" Camera", (const char*)0, false, !cameraSettingsOpen))
					{
						cameraSettingsOpen = true;
					}
					if (ImGui::MenuItem(" Post Processing", (const char*)0, false, !postProcessingOpen))
					{
						postProcessingOpen = true;
					}
					if (ImGui::MenuItem(" Editor Styler", (const char*)0, false, !editorStylerOpen))
					{
						editorStylerOpen = true;
					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu(" Tools"))
				{
					if (ImGui::MenuItem(" Shader Program Editor", (const char*)0, false, !shaderProgramEditorOpen))
					{
						shaderProgramEditorOpen = true;
					}
					if (ImGui::MenuItem(" Material Editor", (const char*)0, false, !materialEditorOpen))
					{
						materialEditorOpen = true;
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

		if (consoleOpen)
		{
			ImGui::Begin("Console", &consoleOpen);

			ConsoleGUI::Draw();

			ImGui::End();
		}

		if (heirarchyOpen)
		{
			ImGui::Begin("Heirarchy", &heirarchyOpen);

			Heirarchy::Draw();

			ImGui::End();
		}

		if (inspectorOpen)
		{
			ImGui::Begin("Inspector", &inspectorOpen);

			inspector->Draw();

			ImGui::End();
		}

		if (sceneViewOpen)
		{
			ImVec2 oldPadding = ImGui::GetStyle().WindowPadding;
			ImGui::GetStyle().WindowPadding = ImVec2(0.0f, 0.0f);

			ImGui::Begin("Scene", &sceneViewOpen);
			
			SceneGUI::DrawScene();

			ImGui::End();

			ImGui::GetStyle().WindowPadding = oldPadding;
		}

		if (shaderProgramEditorOpen)
		{
			ImGui::Begin("Shader Program Editor", &shaderProgramEditorOpen, ImGuiWindowFlags_UnsavedDocument * ShaderProgramGUI::dirty);
			
			ShaderProgramGUI::Draw();
			
			ImGui::End();
		}

		if (materialEditorOpen)
		{
			ImGui::Begin("Material Editor", &materialEditorOpen, ImGuiWindowFlags_UnsavedDocument * MaterialGUI::dirty);
			
			MaterialGUI::Draw();
			
			ImGui::End();
		}

		if (meshCookerOpen)
		{
			ImGui::Begin("Mesh Cooker", &meshCookerOpen);
			
			MeshCooker::Draw();

			ImGui::End();
		}

		if (cameraSettingsOpen)
		{
			ImGui::Begin("Camera", &cameraSettingsOpen);
			
			EditorCameraGUI::DrawEditorCameraGUI((EditorCamera*)mainCamera);
			
			ImGui::End();
		}

		if (editorStylerOpen)
		{
			ImGui::Begin("Editor Styler", &editorStylerOpen);
			
			EditorStyler::Draw();
			
			ImGui::End();
		}

		if (postProcessingOpen)
		{
			ImGui::Begin("Post Processing", &postProcessingOpen);
			
			ImGui::DragFloat("HDR Exposure", &exposure, 0.1f, 0.1f, 1000.0f);

			ImGui::End();
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

	void EditorGUI::FileDropCallback(GLFWwindow* window, int count, const char** filepaths)
	{
		vector<string> files;

		for (int i = 0; i < count; i++)
		{
			files.push_back(filepaths[i]);
		}

		for (int i = 0; i < files.size(); i++)
		{
			if (CheckIfStringContains(files[i], { ".fbx", ".obj", ".gltf" }) != string::npos)
			{
				MeshCooker::fileQueue.push_back(files[i]);
			}
		}
	}
}
