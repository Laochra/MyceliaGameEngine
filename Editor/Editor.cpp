#include "Editor.h"
#include "AppInfo.h"

#include "EditorGUI.h"
#include "EditorCamera.h"
#include "TransformEdit.h"

#include "stb_image.h"

#include "LightingManager.h"

#include "GeneralMacros.h"

void Editor::Initialise()
{
	EditorGUI::Initialise();
	glfwSetDropCallback(AppInfo::window, EditorGUI::FileDropCallback);

	AppInfo::editorCamera = GameObject3D::Instantiate<EditorCamera>(vec3(-0.25f, 1.0f, 1.5f));
	((EditorCamera*)AppInfo::editorCamera)->xRotation = -80;
	((EditorCamera*)AppInfo::editorCamera)->yRotation = -20;

	AppInfo::gameCamera = GameObject3D::Instantiate<Camera>(vec3(0.0f, 0.0f, 0.0f));

	LightingManager::ambientLight = Light(vec3(0.1f, 0.1f, 0.1f));
	LightingManager::directionalLight = DirectionalLight(vec3(1.0f, 1.0f, 1.0f), glm::normalize(vec3(-0.1f, -1, -1)));
	
	AppInfo::input->enabled = false;

	game->transformsDrawFunc = TransformEdit::Draw;
	game->Initialise(&EditorGUI::sceneViewTexture);

	glfwSetWindowTitle(AppInfo::window, StringBuilder(AppInfo::name, " - Editor").CStr());
}

void Editor::FixedUpdate()
{
	Updater::CallFixedUpdate();

	if (AppInfo::state == AppState::Playing)
	{
		game->FixedUpdate();
	}
}
void Editor::Update()
{	
	EditorGUI::Draw();
	Updater::CallUpdate();

	if (AppInfo::state == AppState::Editor)
	{
		TransformEdit::Update();
		vec3 cameraPos(AppInfo::ActiveCamera()->GetPosition());
		AppInfo::debug->lines.AddGrid(vec3((int)cameraPos.x, 0, (int)cameraPos.z), 50);
	}
	if (AppInfo::state == AppState::Playing)
	{
		game->Update();
	}
}
void Editor::Draw()
{
	if (AppInfo::screenWidth <= 0 || AppInfo::screenHeight <= 0)
	{
		EditorGUI::RenderGUI();
		return;
	}

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	// Generate GUID Texture
	if (AppInfo::state == AppState::Editor)
	{
		GameObjectManager::DrawGUIDs(
			EditorGUI::guidFBO,
			EditorGUI::guidTexture,
			EditorGUI::guidDepth,
			TransformEdit::DrawIDs);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	game->displayUI = EditorGUI::displayUI || AppInfo::state != AppState::Editor;

	game->Draw();
	
	EditorGUI::RenderGUI();
}

bool Editor::OnClose()
{
	if (!EditorGUI::CleanUp()) return false;
	
	if (AppInfo::state == AppState::Playing)
	{
		game->OnClose();
	}
	
	del(AppInfo::editorCamera);
	
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	return true;
}
