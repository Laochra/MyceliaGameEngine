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
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;	// Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;	// Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;		// Enable Docking
	
		ImGui_ImplGlfw_InitForOpenGL(window, /*install_callbacks:*/ true);
		ImGui_ImplOpenGL3_Init();
	//End Dear ImGui Setup


	camera = new EditorCamera();

	Gizmos::create(100000, 10000, 0, 0);
}

void Editor::OnFrameStart()
{
	// Ready ImGui For Immediate Mode Rendering
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::ShowDemoWindow();
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
}

void Editor::OnClose()
{
	delete camera;

	Gizmos::destroy();
}