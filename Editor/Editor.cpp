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

#include "EditorDebug.h"

#include "GeneralMacros.h"

void Editor::Initialise()
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

	// ImGui Initialisation
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		//ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;		// Enable Keyboard Controls
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
	mainCamera->SetPosition({ -0.25, 1.0f, 1.5f });
	((EditorCamera*)mainCamera)->xRotation = -80;
	((EditorCamera*)mainCamera)->yRotation = -20;

	debug->lines.AddWorldGrid(10, vec3(0.0f), -FLT_MAX);


	object = GameObject3D::Instantiate<MeshRenderer>(vec3(0.0f, 0.0f, 0.35f), glm::identity<quat>(), vec3(0.01f, 0.01f, 0.01f), vec3(0.0f, -0.5f, 0.0f));
	object->SetName("Table");
	object->SetMesh("Assets\\Meshes\\Table.fbx"); object->SetMaterial("Assets\\Materials\\Table.mat");

	inspector->SetTarget(object);

	LightingManager::ambientLight = Light(vec3(0.1f, 0.1f, 0.1f));

	LightingManager::directionalLight = DirectionalLight(vec3(1.0f, 1.0f, 1.0f), glm::normalize(vec3(-0.1f, -1, -1)));

	MeshRenderer* lightbox = GameObject3D::Instantiate<MeshRenderer>(vec3(0.0f, 1.5f, 0.0f), glm::identity<quat>(), vec3(0.20f, 0.05f, 0.10f), vec3(0.0f, 0.0f, 0.0f));
	lightbox->SetName("Lightbox");
	lightbox->SetMesh("ProceduralCube"); lightbox->SetMaterial("Assets\\Materials\\Wood Planks.mat");
	MeshRenderer* bulb = GameObject3D::Instantiate<MeshRenderer>(vec3(0.0f, -0.6f, 0.0f), glm::identity<quat>(), vec3(0.667f, 0.2f, 0.667f), vec3(0.0f, 0.0f, 0.0f), lightbox);
	bulb->SetName("Bulb");
	bulb->SetMesh("ProceduralCube"); bulb->SetMaterial("Assets\\Materials\\Bulb.mat");
	LightObject* downlight = GameObject3D::Instantiate<LightObject>(vec3(0.0f, 0.6f, 0.0f), glm::identity<quat>(), vec3(1), vec3(0), bulb);
	downlight->SetName("Downlight");
	downlight->LookAt({ 0, 0, 0 });
	downlight->range = 1.0f; downlight->colour = vec3(255 / 255.0f, 228 / 255.0f, 172 / 255.0f); downlight->intensity = 2.4f; downlight->angle[1] = glm::cos(glm::radians(28.0f));
	
	GameObject3D* floors = GameObject3D::Instantiate<GameObject3D>(vec3(0.0f, 0.0f, 0.0f), glm::identity<quat>(), vec3(2.00f, 1.00f, 2.00f), vec3(0.0f, -0.0025f, 0.0f));
	floors->SetName("Floors");
	for (int x = -1; x <= 1; x++)
	{
		for (int y = 0; y <= 1; y++)
		{
			MeshRenderer* floor = GameObject3D::Instantiate<MeshRenderer>(vec3(x, 0.0f, y), glm::identity<quat>(), vec3(1.00f, 1.00f, 1.00f), vec3(0.0f, 0.0f, 0.0f), floors);
			floor->SetName(("Floor " + std::to_string(x) + ", " + std::to_string(y)).c_str());
			floor->SetMesh("ProceduralQuad"); floor->SetMaterial("Assets\\Materials\\Wood Planks.mat");
		}
	}

	GameObject3D* walls = GameObject3D::Instantiate<GameObject3D>(vec3(0.0f, 0.0f, 0.0f), glm::identity<quat>(), vec3(2.00f, 1.00f, 2.00f), vec3(0.0f, -0.0f, 0.502f));
	walls->SetName("Walls");
	walls->Rotate(glm::radians(90.0f), vec3(1, 0, 0));
	for (int x = -1; x <= 1; x++)
	{
		MeshRenderer* wall = GameObject3D::Instantiate<MeshRenderer>(vec3(x, 0.002f, 0.0f), glm::identity<quat>(), vec3(1.00f, 1.00f, 1.00f), vec3(0.0f, 0.0f, 0.0f), walls);
		wall->SetName(("Wall " + std::to_string(x)).c_str());
		wall->SetMesh("ProceduralQuad"); wall->SetMaterial("Assets\\Materials\\Wallpaper.mat");
	}

	input->enabled = false;


	// Initialise ShaderProgramGUI
	ShaderProgramGUI::ValidateLinkage();
	// Initialise MaterialGUI
	MaterialGUI::LoadShaderProgram("PBRLit");
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

	hdrProgram.LoadShader(VertexStage, "Engine\\DefaultAssets\\FullScreenQuad.vert");
	hdrProgram.LoadShader(FragmentStage, "Engine\\DefaultAssets\\HDR.frag");
	hdrProgram.Link();

	blurProgram.LoadShader(VertexStage, "Engine\\DefaultAssets\\FullScreenQuad.vert");
	blurProgram.LoadShader(FragmentStage, "Engine\\DefaultAssets\\BoxBlur.frag");
	blurProgram.Link();

	fxaaProgram.LoadShader(VertexStage, "Engine\\DefaultAssets\\FullScreenQuad.vert");
	fxaaProgram.LoadShader(FragmentStage, "Engine\\DefaultAssets\\FXAA.frag");
	fxaaProgram.Link();
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
				if (ImGui::MenuItem("Post Processing", (const char*)0, false, !postProcessingOpen))
				{
					postProcessingOpen = true;
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
				if (((EditorCamera*)mainCamera)->freeCamera.pressed())
				{
					glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
					if (glfwRawMouseMotionSupported()) glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
					ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse; // Disables Imgui's Mouse Input
				}
				input->enabled = true;
			}
			else if (input->enabled && !((EditorCamera*)mainCamera)->freeCamera.down())
			{
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse; // Re-enables Imgui's Mouse Input
				input->enabled = false;
			}

			if (((EditorCamera*)mainCamera)->freeCamera.released())
			{
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse; // Re-enables Imgui's Mouse Input
				input->enabled = false;
			}


			ImGui::Image((void*)sceneViewColourBufferOutput, ImVec2(screenWidth, screenHeight), ImVec2(0, 1), ImVec2(1, 0));

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

	if (postProcessingOpen)
	{
		ImGui::Begin("Post Processing", &postProcessingOpen);
		{
			ImGui::DragFloat("HDR Exposure", &exposure, 0.1f, 0.1f, 1000.0f);


		} ImGui::End();
	}
}

void Editor::Draw()
{
	if (!sceneViewOpen) return;

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glDeleteFramebuffers(1, &sceneViewFrameBufferHDR);
	glDeleteTextures(2, &sceneViewColourBufferHDR[0]);
	glDeleteRenderbuffers(1, &sceneViewDepthStencilBuffer);

	glGenFramebuffers(1, &sceneViewFrameBufferHDR);
	glBindFramebuffer(GL_FRAMEBUFFER, sceneViewFrameBufferHDR);

	glGenRenderbuffers(1, &sceneViewDepthStencilBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, sceneViewDepthStencilBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, sceneViewDepthStencilBuffer);

	// Main Colour Textures
	glGenTextures(2, &sceneViewColourBufferHDR[0]);
	for (uint texId = 0; texId < 2; texId++)
	{
		glBindTexture(GL_TEXTURE_2D, sceneViewColourBufferHDR[texId]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + texId, GL_TEXTURE_2D, sceneViewColourBufferHDR[texId], 0);
	}
	uint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);

	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, screenWidth, screenHeight);
}

void Editor::DrawPostProcess()
{
	// Draw Debug Gizmos Unaffected by Post Processing
	glDeleteTextures(1, &sceneViewColourBufferGizmos);
	glGenTextures(1, &sceneViewColourBufferGizmos);
	glBindTexture(GL_TEXTURE_2D, sceneViewColourBufferGizmos);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sceneViewColourBufferGizmos, 0);
	uint attachment[1] = { GL_COLOR_ATTACHMENT0 };
	
	glDrawBuffers(1, attachment);

	glEnable(GL_DEPTH_TEST);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	
	debug->lines.Draw();
	
	/// Screen Post Processing
	glDisable(GL_CULL_FACE);
	Mesh screenQuad;
	screenQuad.InitialiseQuad();

	// Bloom Passes
	const int downResFactor = 4;
	glViewport(0, 0, screenWidth / downResFactor, screenHeight / downResFactor);
	uint blurFrameBuffers[2];
	uint blurColourBuffers[2];
	glGenFramebuffers(2, &blurFrameBuffers[0]);
	glGenTextures(2, &blurColourBuffers[0]);
	
	for (int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, blurFrameBuffers[i]);
		glBindTexture(GL_TEXTURE_2D, blurColourBuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenWidth / downResFactor, screenHeight / downResFactor, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurColourBuffers[i], 0);
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_DEPTH_TEST);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	int passes = 4;
	blurProgram.Bind();
	blurProgram.BindUniform("BrightTexture", 0);
	for (uint p = 0; p < passes; p++)
	{
		bool b = p % 2;
		glActiveTexture(GL_TEXTURE0);
		glBindFramebuffer(GL_FRAMEBUFFER, blurFrameBuffers[b]);
		glBindTexture(GL_TEXTURE_2D, p == 0 ? sceneViewColourBufferHDR[1] : blurColourBuffers[!b]);

		screenQuad.Draw();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, screenWidth, screenHeight);


	// Normalising HDR for Screen
	unsigned int ldrFrameBuffer;
	unsigned int ldrColourBuffer;
	glGenFramebuffers(1, &ldrFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, ldrFrameBuffer);

	glGenTextures(1, &ldrColourBuffer);
	glBindTexture(GL_TEXTURE_2D, ldrColourBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ldrColourBuffer, 0);

	glClearColor(0.8f, 0.75f, 0.85f, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	hdrProgram.Bind();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sceneViewColourBufferHDR[0]);
	hdrProgram.BindUniform("HDRTexture", 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, blurColourBuffers[passes % 2]);
	hdrProgram.BindUniform("BloomTexture", 1);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, sceneViewColourBufferGizmos);
	hdrProgram.BindUniform("GizmosTexture", 2);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, ldrColourBuffer);
	hdrProgram.BindUniform("CurrentColourBuffer", 3);
	hdrProgram.BindUniform("Exposure", exposure);

	screenQuad.Draw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDeleteFramebuffers(2, &blurFrameBuffers[0]);
	glDeleteTextures(2, &blurColourBuffers[0]);

	// FXAA
	glDeleteFramebuffers(1, &sceneViewFrameBufferOutput);
	glDeleteTextures(1, &sceneViewColourBufferOutput);

	glGenFramebuffers(1, &sceneViewFrameBufferOutput);
	glBindFramebuffer(GL_FRAMEBUFFER, sceneViewFrameBufferOutput);

	glGenTextures(1, &sceneViewColourBufferOutput);
	glBindTexture(GL_TEXTURE_2D, sceneViewColourBufferOutput);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sceneViewColourBufferOutput, 0);

	fxaaProgram.Bind();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ldrColourBuffer);
	fxaaProgram.BindUniform("AliasedTexture", 0);

	glClear(GL_COLOR_BUFFER_BIT);

	screenQuad.Draw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDeleteFramebuffers(1, &ldrFrameBuffer);
	glDeleteTextures(1, &ldrColourBuffer);
}

void Editor::DrawGUI()
{
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

void Editor::OnClose()
{
	del(mainCamera);

	del(gameObjectManager);
	del(inspector);

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}