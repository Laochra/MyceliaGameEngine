#include "Editor.h"
#include "AppInfo.h"

#include "EditorGUI.h"
#include "EditorCamera.h"
#include "TransformEdit.h"

#include "stb_image.h"

#include "LightingManager.h"

#include "GeneralMacros.h"

void static RadialInteractionHandler(uint selection)
{
	currentTileType = (HexType)(selection + 1);
}

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
	
	
	hexGrid = GameObject::Instantiate<LinkedHexGrid>();

	hexGrid->AddTile(vec3(0, 0, 0), HexDir::NorthWest);
	hexGrid->AddTile(vec3(0, 0, 0), HexDir::North);
	hexGrid->AddTile(vec3(0, 0, 0), HexDir::NorthEast);
	hexGrid->AddTile(vec3(0, 0, 0), HexDir::SouthEast);
	hexGrid->AddTile(vec3(0, 0, 0), HexDir::South);
	hexGrid->AddTile(vec3(0, 0, 0), HexDir::SouthWest);

	radialMenu = new RadialMenu();
	radialMenu->interactionHandler = { (RadialMenu::InteractFunc)RadialInteractionHandler };
	
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
		
		if (AppInfo::input->screenCursorPos.x >= 0 &&
			AppInfo::input->screenCursorPos.y >= 0 &&
			AppInfo::input->screenCursorPos.x < screenWidth &&
			AppInfo::input->screenCursorPos.y < screenHeight)
		{
			if (!radialMenu->enabled && AppInfo::input->GetKeyPressed(KeyCode::MouseLeft))
			{
				const int pixelCount = screenWidth * screenHeight;
				glm::ivec4 * hexPosPixels = new glm::ivec4[pixelCount];
				glGetTextureImage(hexPosTexture, 0, GL_RGBA_INTEGER, GL_INT, pixelCount * sizeof(glm::ivec4), hexPosPixels);
				uint hexPosIndex = (int)AppInfo::input->screenCursorPos.x + (int)AppInfo::input->screenCursorPos.y * screenWidth;
				glm::ivec4 hexPos = hexPosPixels[hexPosIndex];
				if (hexPos.a != 0)
				{
					hexGrid->UpdateTile((glm::ivec2)hexPos, currentTileType);
				}
			}
			
			if (AppInfo::input->GetKeyPressed(KeyCode::Space)) radialMenu->enabled = true;
			
			vec2 cursorNormalised = AppInfo::input->screenCursorPos / vec2(screenWidth, screenHeight) * 2.0f - 1.0f;
			float aspect = 1.0f / (screenHeight == 0 ? 0.0f : (screenWidth / (float)screenHeight));
			radialMenu->Update(vec2(cursorNormalised.x / aspect, cursorNormalised.y), Keybind(KeyCode::MouseLeft));
		}
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
	GameObjectManager::DrawGUIDs(
		EditorGUI::guidFBO,
		EditorGUI::guidTexture,
		EditorGUI::guidDepth,
		TransformEdit::DrawIDs);
	
	game->Draw();
	
	EditorGUI::RenderGUI();
}

bool Editor::OnClose()
{
	if (!EditorGUI::CleanUp()) return false;
	
	del(radialMenu);
	
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
