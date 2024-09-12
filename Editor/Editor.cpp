#include "Editor.h"
#include "AppInfo.h"

#include "EditorGUI.h"
#include "EditorCamera.h"
#include "TransformEdit.h"

#include "stb_image.h"

#include "LightingManager.h"
#include "AudioManager.h"

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

	if (AppInfo::CompareState(AppState::Playing))
	{
		game->FixedUpdate();
	}
}
void Editor::Update()
{	
	EditorGUI::Draw();
	Updater::CallUpdate();

	if (AppInfo::CompareState(AppState::Editor))
	{
		TransformEdit::Update();
		vec3 cameraPos(AppInfo::ActiveCamera()->GetPosition());
		AppInfo::debug->lines.AddGrid(vec3((int)cameraPos.x, 0, (int)cameraPos.z), 50);
	}
	if (AppInfo::CompareState(AppState::Playing))
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
	if (AppInfo::CompareState(AppState::Editor))
	{
		GameObjectManager::DrawGUIDs(
			EditorGUI::guidFBO,
			EditorGUI::guidTexture,
			EditorGUI::guidDepth,
			TransformEdit::DrawIDs);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	game->displayUI = EditorGUI::displayUI || !AppInfo::CompareState(AppState::Editor);

	game->Draw();
	
	EditorGUI::RenderGUI();
}

bool Editor::OnClose()
{
	if (!EditorGUI::CleanUp()) return false;
	
	if (AppInfo::CompareState(AppState::Playing))
	{
		game->OnStop();
	}

	game->OnClose();
	
	del(AppInfo::editorCamera);
	
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	return true;
}

void Editor::OnStateChange(AppState newState)
{
	switch (AppInfo::GetState())
	{
	case AppState::Editor:
		switch (newState)
		{
		case AppState::Playing:	game->OnStart();		break;
		default:													break;
		}
		break;
	case AppState::Playing:
		switch (newState)
		{
		case AppState::Editor:	game->OnStop();		break;
		case AppState::Paused:	game->OnPause();		break;
		default:													break;
		}
		break;
	case AppState::Paused:
		switch (newState)
		{
		case AppState::Editor:	game->OnStop();		break;
		case AppState::Playing: game->OnUnpause();	break;
		default:													break;
		}
		break;
	}
}
