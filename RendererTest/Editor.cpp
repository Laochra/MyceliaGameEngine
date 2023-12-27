#include "Editor.h"

#include "ImGuiIncludes.h"

#include "Time.h"
#include "Input.h"
#include "UpdateList.h"

#include "EditorCamera.h"
#include "EditorCameraConfig.h"

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
	EditorCameraConfig::Load((EditorCamera*)camera);

	Gizmos::create(100000, 10000, 0, 0);

	shader.LoadShader(VertexStage, "Simple.vert");
	shader.LoadShader(FragmentStage, "Simple.frag");

	if (shader.Link() == false)
	{
		std::cout << "Shader Error: " << shader.GetLastError();
		return;
	}

	mesh.InitialiseCube();

	meshTransform =
	{
		2, 0, 0, 0,
		0, 2, 0, 0,
		0, 0, 2, 0,
		0, 0, 0, 1
	};

	light.colour = { 1, 0.9f, 0.8f };
	light.direction = glm::normalize(vec3{ 0, -0.7, -0.7 });
	ambientLight = { 0.25f, 0.25f, 0.25f };

	meshTexture.Load("shroom.png");
}

void Editor::OnFrameStart()
{
	// Ready ImGui For Immediate Mode Rendering
	if (applicationFocused)
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		//ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
		//ImGui::ShowDemoWindow();
	}

	UpdateList::OnFrameStart();
}

void Editor::FixedUpdate()
{
	UpdateList::FixedUpdate();
}

void Editor::Update()
{
	Keybind freeCamera(MouseRight);

	if (freeCamera.pressed())
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	else if (freeCamera.released())
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	if (!applicationFocused) return;
	ImGui::Begin("Camera Settings");
	{
		ImGui::BeginTabBar("Camera Settings Tab Bar");
		{
			if (ImGui::BeginTabItem("Display"))
			{
				ImGui::SeparatorText("Clipping Planes");
				ImGui::InputFloat("Near", &camera->nearClip);
				ImGui::InputFloat("Far", &camera->farClip);

				ImGui::SeparatorText("FOV");
				float fovDegrees = glm::degrees(camera->fov);
				ImGui::InputFloat("(degrees)", &fovDegrees);
				camera->fov = glm::radians(fovDegrees);

				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Movement"))
			{
				ImGui::SeparatorText("Position");
				ImGui::InputFloat("x", &camera->position.x);
				ImGui::InputFloat("y", &camera->position.y);
				ImGui::InputFloat("z", &camera->position.z);

				ImGui::SeparatorText("Rotation");
				ImGui::InputFloat("x ", &((EditorCamera*)camera)->xRotation);
				ImGui::InputFloat("y ", &((EditorCamera*)camera)->yRotation);

				ImGui::SeparatorText("Fly Speed");
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
				ImGui::Spacing(); ImGui::Spacing();
				ConfigureKeybind("Free Camera", &((EditorCamera*)camera)->freeCamera);

				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();

		ImGui::Spacing(); ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing(); ImGui::Spacing();

		if (ImGui::Button("Save Settings"))
		{
			EditorCameraConfig::Save((EditorCamera*)camera);
		}
	}
	ImGui::End();

	meshTransform = glm::rotate(meshTransform, glm::radians(0.5f), vec3(0, 1, 0));

	//ImGui::Begin("Light Settings");
	//	ImGui::SeparatorText("Direction");
	//	ImGui::InputFloat("X Direction", &light.direction.x);
	//	ImGui::InputFloat("Y Direction", &light.direction.y);
	//	ImGui::InputFloat("Z Direction", &light.direction.z);
	//	light.direction = glm::normalize(light.direction);
	//ImGui::End();

	//light.direction = glm::normalize(vec3(glm::cos(Time::time * 2), glm::sin(Time::time * 2), 0));

	UpdateList::Update();
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

	mat4 ProjectionViewMatrix = camera->GetProjectionMatrix(w, h) * camera->GetViewMatrix();

	Gizmos::draw(ProjectionViewMatrix);

	// Bind Shader
	shader.Bind();

	// Bind Light
	shader.BindUniform("AmbientColour", ambientLight);
	shader.BindUniform("LightColour", light.colour);
	shader.BindUniform("LightDirection", light.direction);

	// Bind Transform
	shader.BindUniform("ProjectionViewModel", ProjectionViewMatrix * meshTransform);

	// Bind Transform for Lighting
	shader.BindUniform("ModelMatrix", meshTransform);

	meshTexture.Bind(0);
	shader.BindUniform("diffuseTex", 0);

	// Draw Quad
	mesh.Draw();

	// Draw ImGui UI
	if (applicationFocused)
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(window);
		}
	}

	UpdateList::Draw();
}

void Editor::OnClose()
{
	delete camera;

	Gizmos::destroy();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}