#include "Editor.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "Time.h"
#include "Input.h"
#include "EditorCamera.h"

using glm::vec4;
using glm::mat4;

void Editor::Initialise()
{
	// Begin Dear ImGui Setup
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;	// Enable Keyboard Controls
		//ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;	// Enable Gamepad Controls
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;		// Enable Docking
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;		// Enable Viewports
	
		ImGui_ImplGlfw_InitForOpenGL(window, /*install_callbacks:*/ true);
		ImGui_ImplOpenGL3_Init();

	camera = new EditorCamera();

	Gizmos::create(100000, 10000, 0, 0);
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
	Keybind freeCameraToggle(MouseRight);

	if (freeCameraToggle.pressed())
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	else if (freeCameraToggle.released())
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	ImGui::Begin("Camera Settings");
	{
		ImGui::BeginTabBar("Camera Settings Tab Bar");
		{
			if (ImGui::BeginTabItem("Display"))
			{
				ImGui::Text("Clipping Planes");
				ImGui::InputFloat("Near", &camera->nearClip);
				ImGui::InputFloat("Far", &camera->farClip);

				ImGui::Separator();

				ImGui::Text("FOV");
				ImGui::InputFloat("(radians)", &camera->fov);

				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Movement"))
			{
				ImGui::Text("Position");
				ImGui::InputFloat("x", &camera->position.x);
				ImGui::InputFloat("y", &camera->position.y);
				ImGui::InputFloat("z", &camera->position.z);

				ImGui::Separator();

				ImGui::Text("Rotation");
				ImGui::InputFloat("x ", &((EditorCamera*)camera)->xRotation);
				ImGui::InputFloat("y ", &((EditorCamera*)camera)->yRotation);

				ImGui::Separator();

				ImGui::Text("Fly Speed");
				ImGui::InputFloat("Regular", &((EditorCamera*)camera)->flySpeed);
				ImGui::InputFloat("Quick", &((EditorCamera*)camera)->quickFlySpeed);

				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Controls"))
			{
				ImGui::SeparatorText("Movement");

				ConfigureKeyAxis("Back", "Forward", &((EditorCamera*)camera)->zInput);
				ImGui::Spacing(); ImGui::Spacing();
				ConfigureKeyAxis("Left", "Right", &((EditorCamera*)camera)->xInput);
				ImGui::Spacing(); ImGui::Spacing();
				ConfigureKeyAxis("Down", "Up", &((EditorCamera*)camera)->yInput);

				ImGui::SeparatorText("Other");

				ConfigureKeybind("Quick Fly", &((EditorCamera*)camera)->quickMode);

				ImGui::Separator();

				ConfigureKeybind("Free Camera", &((EditorCamera*)camera)->freeCameraToggle);

				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();
	}
	ImGui::End();
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

	int w = 0, h = 0;
	glfwGetFramebufferSize(window, &w, &h);

	glViewport(0, 0, w, h);

	Gizmos::draw(camera->GetProjectionMatrix(w, h) * camera->GetViewMatrix());


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
	delete camera;

	Gizmos::destroy();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}