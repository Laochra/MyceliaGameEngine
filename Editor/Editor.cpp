#include "Editor.h"

#include "ImGuiIncludes.h"

#include "EditorCamera.h"
#include "EditorCameraGUI.h"

#include "MeshRendererGUI.h"

#include "ShaderProgramGUI.h"
#include "MaterialGUI.h"

#include "ConsoleGUI.h"
#include "Heirarchy.h"

#include "Inspector.h"

#include "EditorStyler.h"

#include "TimeManager.h"

#include "stb/stb_image.h"

void Editor::Initialise()
{
	glfwSetWindowTitle(window, "Editor");

	GLFWimage icons[4];
	icons[0].pixels = stbi_load("Engine\\Icons\\shroom16x16.png", &icons[0].width, &icons[0].height, 0, 4);
	icons[1].pixels = stbi_load("Engine\\Icons\\shroom32x32.png", &icons[1].width, &icons[1].height, 0, 4);
	icons[2].pixels = stbi_load("Engine\\Icons\\shroom48x48.png", &icons[2].width, &icons[2].height, 0, 4);
	icons[3].pixels = stbi_load("Engine\\Icons\\shroom64x64.png", &icons[3].width, &icons[3].height, 0, 4);
	glfwSetWindowIcon(window, 4, icons);
	stbi_image_free(icons[0].pixels);
	stbi_image_free(icons[1].pixels);
	stbi_image_free(icons[2].pixels);
	stbi_image_free(icons[3].pixels);

	// ImGui Initialisation
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;		// Enable Keyboard Controls
		//ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;	// Enable Gamepad Controls
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;			// Enable Docking
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;		// Enable Viewports

		ImGui_ImplGlfw_InitForOpenGL(window, /*install_callbacks:*/ true);
		ImGui_ImplOpenGL3_Init();

		ImGui::GetIO().IniFilename = "Engine\\GUIState.ini";
	}

	gameObjectManager = new GameObjectManager();
	inspector = new Inspector();

	mainCamera = GameObject3D::Instantiate<EditorCamera>(vec3(0.0f, 10.0f, 10.0f));

	Gizmos::create(100000, 10000, 0, 0);

	object = GameObject3D::Instantiate<MeshRenderer>(vec3(0.0f, 0.0, 0.0f), glm::identity<quat>(), vec3(1.0f, 1.0f, 1.0f), vec3(0, -0.5f, 0));
	//object->Rotate(glm::radians(90.0f), vec3(-1, 0, 0));
	//object->SetMesh("Assets\\Meshes\\Marill.fbx");
	//object->SetMaterial("Assets\\Materials\\Marill.mat");
	//object->SetName("Marill");

	inspector->SetTarget(object);

	LightingManager::ambientLight.colour = vec3(0.1f, 0.1f, 0.1f);

	LightingManager::directionalLight.colour = vec3(1.0f, 1.0f, 1.0f);
	LightingManager::directionalLight.direction = glm::normalize(vec3(0, -0.0, -0.7));

	LightingManager::pointLight.colour = vec3(0.9f, 0.0f, 0.0f);
	LightingManager::pointLight.position = vec3(0, 1, 2);
	LightingManager::pointLight.range = 10.0f;

	input->enabled = false;


	// Initialise ShaderProgramGUI
	ShaderProgramGUI::ValidateLinkage();
	// Initialise MaterialGUI
	MaterialGUI::LoadShaderProgram("Default");
	MaterialGUI::dirty = false;

	// ImGui Styling Setup (Serialise and load this. Hardcoding bad)
	{
		ImGuiStyle* style = &ImGui::GetStyle();

		ImFont* odudoMono = ImGui::GetIO().Fonts->AddFontFromFileTTF("Assets\\Fonts\\OdudoMono-Regular.otf", 16);

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

void Editor::OnFrameStart()
{
	// Ready ImGui For Immediate Mode Rendering

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

	//ImGui::ShowDemoWindow();
}

void Editor::FixedUpdate()
{
	
}

void Editor::Update()
{
	if (*object == GameObject::Active)
	{
		//object->LookTowards(mainCamera->GetPosition(), glm::radians(15.0f) * Time::delta);
	}

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New Scene", "Ctrl+N"))
			{

			}
			if (ImGui::MenuItem("Open Scene", "Ctrl+O"))
			{

			}
			if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
			{

			}
			if (ImGui::MenuItem("Save Scene As", "Ctrl+Shift+S"))
			{

			}
			ImGui::EndMenu();
		}
		ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
		if (ImGui::BeginMenu("Windows"))
		{
			if (ImGui::MenuItem("Console", (const char*)0, false, !consoleOpen))
			{
				consoleOpen = true;
			}
			if (ImGui::MenuItem("Heirarchy", (const char*)0, false, !heirarchyOpen))
			{
				heirarchyOpen = true;
			}
			if (ImGui::MenuItem("Inspector", (const char*)0, false, !inspectorOpen))
			{
				inspectorOpen = true;
			}
			if (ImGui::MenuItem("Scene View", (const char*)0, false, !sceneViewOpen))
			{
				sceneViewOpen = true;
			}

			if (ImGui::BeginMenu("Settings"))
			{
				if (ImGui::MenuItem("Camera", (const char*)0, false, !cameraSettingsOpen))
				{
					cameraSettingsOpen = true;
				}
				if (ImGui::MenuItem("Editor Styler", (const char*)0, false, !editorStylerOpen))
				{
					editorStylerOpen = true;
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Tools"))
			{
				if (ImGui::MenuItem("Shader Program Editor", (const char*)0, false, !shaderProgramEditorOpen))
				{
					shaderProgramEditorOpen = true;
				}
				if (ImGui::MenuItem("Material Editor", (const char*)0, false, !materialEditorOpen))
				{
					materialEditorOpen = true;
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

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
		{
			screenWidth = ImGui::GetWindowWidth();
			float titleBarHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2;
			screenHeight = ImGui::GetWindowHeight() - titleBarHeight;
			
			if (ImGui::IsWindowHovered())
			{
				input->enabled = true;

				if (((EditorCamera*)mainCamera)->freeCamera.pressed())
				{
					glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
					if (glfwRawMouseMotionSupported()) glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
				}
			}
			else if (input->enabled && !((EditorCamera*)mainCamera)->freeCamera.down())
			{
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				if (glfwRawMouseMotionSupported()) glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);

				input->enabled = false;
			}

			if (((EditorCamera*)mainCamera)->freeCamera.released())
			{
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				if (glfwRawMouseMotionSupported()) glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);

				input->enabled = false;
			}

			ImGui::Image((void*)sceneViewColourBuffer, ImVec2(screenWidth, screenHeight), ImVec2(0, 1), ImVec2(1, 0));

		} ImGui::End();
		ImGui::GetStyle().WindowPadding = oldPadding;
	}

	if (shaderProgramEditorOpen)
	{
		if (ShaderProgramGUI::dirty == false) ImGui::Begin("Shader Program Editor", &shaderProgramEditorOpen);
		else ImGui::Begin("Shader Program Editor", &shaderProgramEditorOpen, ImGuiWindowFlags_UnsavedDocument);
		{
			ShaderProgramGUI::Draw();
		} ImGui::End();
	}

	if (materialEditorOpen)
	{
		if (MaterialGUI::dirty == false) ImGui::Begin("Material Editor", &materialEditorOpen);
		else ImGui::Begin("Material Editor", &materialEditorOpen, ImGuiWindowFlags_UnsavedDocument);
		{
			MaterialGUI::Draw();
		} ImGui::End();
	}

	if (cameraSettingsOpen)
	{
		ImGui::Begin("Camera", &cameraSettingsOpen);
		{
			EditorCameraGUI::DrawEditorCameraGUI((EditorCamera*)mainCamera);
		} ImGui::End();
	}

	if (editorStylerOpen)
	{
		ImGui::Begin("Editor Styler", &editorStylerOpen);
		{
			EditorStyler::Draw();
		} ImGui::End();
	}
}

void Editor::Draw()
{
	if (!sceneViewOpen) return;

	glDeleteFramebuffers(1, &sceneViewFrameBuffer);
	glDeleteTextures(1, &sceneViewColourBuffer);
	glDeleteRenderbuffers(1, &sceneViewDepthStencilBuffer);

	glGenFramebuffers(1, &sceneViewFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, sceneViewFrameBuffer);

	glGenTextures(1, &sceneViewColourBuffer);
	glBindTexture(GL_TEXTURE_2D, sceneViewColourBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sceneViewColourBuffer, 0);

	glGenRenderbuffers(1, &sceneViewDepthStencilBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, sceneViewDepthStencilBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, sceneViewDepthStencilBuffer);

	glClearColor(0.8f, 0.75f, 0.85f, 1);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Gizmos::clear();
	
	Gizmos::addTransform(mat4(1));
	
	vec4 white(1);
	vec4 black(0, 0, 0, 1);
	
	for (int i = 0; i < 21; i++)
	{
		Gizmos::addLine(vec3(-10 + i, 0, 10), vec3(-10 + i, 0, -10), i == 10 ? white : black);
	
		Gizmos::addLine(vec3(-10, 0, -10 + i), vec3(10, 0, -10 + i), i == 10 ? white : black);
	}

	//glfwGetFramebufferSize(window, &screenWidth, &screenHeight);

	glViewport(0, 0, screenWidth, screenHeight);



	mat4 ProjectionViewMatrix = mainCamera->GetProjectionMatrix((float)screenWidth, (float)screenHeight) * mainCamera->GetViewMatrix();

	Gizmos::draw(ProjectionViewMatrix);
}

void Editor::DrawGUI()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor(0.2f, 0.1f, 0.25f, 1);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

void Editor::OnClose()
{
	delete mainCamera;

	delete gameObjectManager;
	delete inspector;

	Gizmos::destroy();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}