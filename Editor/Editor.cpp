#include "Editor.h"

#include "ImGuiIncludes.h"

#include "EditorCamera.h"
#include "EditorCameraGUI.h"

#include "MeshRendererGUI.h"

#include "ShaderProgramGUI.h"

#include "TimeManager.h"

void Editor::Initialise()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;	// Enable Keyboard Controls
	//ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;	// Enable Gamepad Controls
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;		// Enable Docking
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;		// Enable Viewports
	
	ImGui_ImplGlfw_InitForOpenGL(window, /*install_callbacks:*/ true);
	ImGui_ImplOpenGL3_Init();

	mainCamera = GameObject3D::Instantiate<EditorCamera>(vec3(0.0f, 10.0f, 10.0f));

	Gizmos::create(100000, 10000, 0, 0);

	object = GameObject3D::Instantiate<MeshRenderer>(vec3(0, 0, 0), quat(), vec3(3, 3, 3), vec3(0, -0.5f, 0));

	LightingManager::light.colour = vec3(1, 0.9f, 0.8f);
	LightingManager::light.direction = glm::normalize(vec3(0, -0.7, -0.7));
	LightingManager::ambientLight = vec3(0.25f, 0.25f, 0.25f);
}

void Editor::OnFrameStart()
{
	// Ready ImGui For Immediate Mode Rendering

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	//ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
	//ImGui::ShowDemoWindow();
}

void Editor::FixedUpdate()
{
	
}

void Editor::Update()
{
	if (ImGui::GetIO().WantCaptureMouse) input->enabled = false;
	else input->enabled = true;

	if (((EditorCamera*)mainCamera)->freeCamera.pressed())
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	else if (((EditorCamera*)mainCamera)->freeCamera.released())
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	if (*object == GameObject::Active)
	{
		object->LookTowards(mainCamera->GetPosition(), glm::radians(15.0f) * Time::delta);
	}
	

	ImGui::Begin("Camera Settings");
		EditorCameraGUI::Draw((EditorCamera*)mainCamera);
	ImGui::End();

	ImGui::Begin("Inspector");
		MeshRendererGUI::Draw(object);
	ImGui::End();

	ImGui::Begin("Shader Program Editor");
		ShaderProgramGUI::Draw();
	ImGui::End();

	//ImGui::Begin("Light Settings");
	//	ImGui::SeparatorText("Direction");
	//	ImGui::InputFloat("X Direction", &light.direction.x);
	//	ImGui::InputFloat("Y Direction", &light.direction.y);
	//	ImGui::InputFloat("Z Direction", &light.direction.z);
	//	light.direction = glm::normalize(light.direction);
	//ImGui::End();
}

void Editor::Draw()
{
	Gizmos::clear();

	Gizmos::addTransform(mat4(1));

	vec4 white(1);
	vec4 black(0, 0, 0, 1);

	for (int i = 0; i < 21; i++)
	{
		Gizmos::addLine(vec3(-10 + i, 0, 10), vec3(-10 + i, 0, -10), i == 10 ? white : black);

		Gizmos::addLine(vec3(-10, 0, -10 + i), vec3(10, 0, -10 + i), i == 10 ? white : black);
	}

	glfwGetFramebufferSize(window, &screenWidth, &screenHeight);

	glViewport(0, 0, screenWidth, screenHeight);

	mat4 ProjectionViewMatrix = mainCamera->GetProjectionMatrix(screenWidth, screenHeight) * mainCamera->GetViewMatrix();

	Gizmos::draw(ProjectionViewMatrix);
}

void Editor::DrawGUI()
{
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

	Gizmos::destroy();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}