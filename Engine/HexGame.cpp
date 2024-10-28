#include "HexGame.h"
#include "AppInfo.h"

#include "LightingManager.h"
#include "GameObjectManager.h"
#include "AudioManager.h"
#include "UIManager.h"
#include "ShaderManager.h"

#include "MeshRenderer.h"
#include "Shader.h"
#include "Camera.h"

#include "StringBuilder.h"

#include "HexProgression.h"
#include "HexAudio.h"
#include "HexRadial.h"
#include "HexScrapbook.h"
#include "HexFog.h"

#include "HabitatData.h"

#include "HexGameSerialiser.h"

#include "RandomGen.h"

#include "Application.h"
#include "Debug.h"

void static RadialInteractionHandler(int selection)
{
	if (selection >= 0)
	{
		currentTileType = &TileData::Get(HexType(currentRadialPage));
		currentTileVariant = selection;

		if (TileData::selectedTile != nullptr)
		{
			string& spriteFilepath = TileData::Get(HexType(currentRadialPage), currentTileVariant).spriteFilepath;
			if (spriteFilepath != "None")
			{
				TileData::selectedTile->Load(spriteFilepath.c_str());
			}
		}
	}
	
	((HexGame*)AppInfo::application->game)->SetState(HexGame::State::Place);
}

static void RefreshUI(uint& uiFBO, uint& uiTexture)
{
	if (uiFBO == 0)
	{
		glGenFramebuffers(1, &uiFBO);
		glGenTextures(1, &uiTexture);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, uiFBO);

	glBindTexture(GL_TEXTURE_2D, uiTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, AppInfo::screenWidth, AppInfo::screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, uiTexture, 0);
}
static void DrawUI(RadialMenu* radialMenu, vector<UISprite*> sprites, uint& uiFBO, uint& uiTexture)
{
	if (uiFBO == 0 || AppInfo::screenSizeJustChanged)
	{
		RefreshUI(uiFBO, uiTexture);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, uiFBO);
	
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);
	for (uint i = 0; i < (uint)sprites.size(); i++)
	{
		if (sprites[i] != nullptr) sprites[i]->Draw();
	}
	UIManager::Sort();
	for (uint i = 0; i < (uint)UIManager::sprites.size(); i++)
	{
		if (UIManager::sprites[i] != nullptr) UIManager::sprites[i]->Draw();
	}
	
	if (radialMenu != nullptr) radialMenu->Draw();

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
}

void HexGame::SetState(HexGame::State newState) noexcept
{
	switch (gameState) // Clean up old state
	{
	case HexGame::State::Place:
	{
		break;
	}
	case HexGame::State::Radial:
	{
		switch (currentRadialPage)
		{
		default: treeRadial->enabled = false;		break;
		case 1U: flowerRadial->enabled = false;	break;
		case 2U: waterRadial->enabled = false;		break;
		case 3U: landRadial->enabled = false;		break;
		}
		HexAudio::PlayMiscSFX(HexAudio::SoundEffect::RadialClose);
		break;
	}
	case HexGame::State::Scrapbook:
	{
		HexScrapbook::Close();
		HexAudio::PlayMiscSFX(HexAudio::SoundEffect::ScrapbookClose);
		break;
	}
	case HexGame::State::StickerEvent:
	{
		currentEvent = nullptr;
		break;
	}
	default: Debug::LogWarning("Unaccounted for game state", locationinfo); break;
	}

	gameState = newState;
	
	switch (newState) // Initialise new state
	{
	case HexGame::State::Place:
	{
		break;
	}
	case HexGame::State::Radial:
	{
		switch (currentRadialPage)
		{
		default: treeRadial->enabled = true;		break;
		case 1U: flowerRadial->enabled = true;		break;
		case 2U: waterRadial->enabled = true;		break;
		case 3U: landRadial->enabled = true;		break;
		}
		HexAudio::PlayMiscSFX(HexAudio::SoundEffect::RadialOpen);
		break;
	}
	case HexGame::State::Scrapbook:
	{
		HexScrapbook::Open();
		HexAudio::PlayMiscSFX(HexAudio::SoundEffect::ScrapbookOpen);
		break;
	}
	case HexGame::State::StickerEvent:
	{
		break;
	}
	default: Debug::LogWarning("Unaccounted for game state", locationinfo); break;
	}
}

static void RefreshOutline(
	vector<ShaderProgram*>& programs,
	vector<uint*>& framebuffers,
	vector<uint*>& textures) noexcept
{
	if (textures[3] == nullptr) return; // Return if just rendering to the backbuffer

	const ShaderProgram& outlineProgram = *programs[0];

	uint& outlineFBO = *framebuffers[0];

	const uint& colourTexture = *textures[0];
	const uint& positionTexture = *textures[1];
	const uint& idTexture = *textures[2];
	uint& outlinedTexture = *textures[3];

	if (outlineFBO == 0)
	{
		glGenFramebuffers(1, &outlineFBO);
		glGenTextures(1, &outlinedTexture);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, outlineFBO);

	glBindTexture(GL_TEXTURE_2D, outlinedTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, AppInfo::screenWidth, AppInfo::screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, outlinedTexture, 0);
}
static void DrawOutline(
	vector<ShaderProgram*>& programs,
	vector<uint*>& framebuffers,
	vector<uint*>& textures) noexcept
{
	ShaderProgram& outlineProgram = *programs[0];

	uint& outlineFBO = *framebuffers[0];

	const uint& colourTexture = *textures[0];
	const uint& positionTexture = *textures[1];
	const uint& idTexture = *textures[2];
	uint& outlinedTexture = *textures[3];

	glViewport(0, 0, AppInfo::screenWidth, AppInfo::screenHeight);

	// If not intentionally targetting the back buffer initialise a new one
	if (textures[3] != nullptr && outlineFBO == 0)
	{
		glGenFramebuffers(1, &outlineFBO);
		glGenTextures(1, &outlinedTexture);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, outlineFBO);

	glClearColor(0.8f, 0.75f, 0.85f, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	outlineProgram.Bind();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colourTexture);
	outlineProgram.BindUniform("ColourMap", 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, positionTexture);
	outlineProgram.BindUniform("PositionMap", 1);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, idTexture);
	outlineProgram.BindUniform("IDMap", 2);
	outlineProgram.BindUniform("PixelSize", 1.0f / vec2(AppInfo::screenWidth, AppInfo::screenHeight));

	PostProcess::Defaults::screenQuad->Draw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

static void RecursiveSetHighlightColour(GameObject3D* gameObject, vec4 colour)
{
	if (gameObject->GetClassID() == MeshRenderer::classID)
	{
		((MeshRenderer*)gameObject)->highlightColour = colour;
	}

	for (GameObject3D* child : *gameObject->GetChildren())
	{
		RecursiveSetHighlightColour(child, colour);
	}
}
const float tileRaiseAmount = 0.1f;
static void RaiseTile(vec2& hoveredPosition, GameObject3D*& selectedGameObject, HexGrid* hexGrid)
{
	HexCubeCoord hoveredCubeCoord = HexCubeCoord::GetFromPos(hoveredPosition);
	vec2 partialCubeCoord = HexCubeCoord::GetFromPosPartial(hoveredPosition);
	float magnitude = HexCubeCoord::GetMagnitudePartial(partialCubeCoord);

	if (magnitude > (float)HexProgression::currentRadius)
	{
		partialCubeCoord = (partialCubeCoord / magnitude) * (float)HexProgression::currentRadius;
		hoveredPosition = HexCubeCoord::ToPosPartial(partialCubeCoord);
		hoveredCubeCoord = HexCubeCoord::GetFromPos(hoveredPosition);
	}

	HexOffsetCoord hoveredHexCoord = HexCubeToOffset(hoveredCubeCoord, hexGrid->centre);
	if (hoveredHexCoord == hexGrid->centre)
	{
		if (selectedGameObject != nullptr)
		{
			selectedGameObject->Translate(vec3(0, -tileRaiseAmount, 0));
			RecursiveSetHighlightColour(selectedGameObject, vec4());
			selectedGameObject = nullptr;
		}
	}
	else
	{
		HexTile& hoveredHex = hexGrid->Get(hoveredHexCoord);
		GameObject3D* hoveredGameObject = nullptr;

		if (hoveredHex.habitat < 0)
		{
			hoveredGameObject = hoveredHex.object;
		}
		else
		{
			for (Habitat& habitat : hexGrid->habitats)
			{
				if (habitat.habitatID == hoveredHex.habitat)
				{
					hoveredGameObject = habitat.object;
					break;
				}
			}
		}

		if (selectedGameObject != hoveredGameObject)
		{
			if (selectedGameObject != nullptr)
			{
				selectedGameObject->Translate(vec3(0, -tileRaiseAmount, 0));
				RecursiveSetHighlightColour(selectedGameObject, vec4());
			}
			if (hoveredGameObject != nullptr)
			{
				hoveredGameObject->Translate(vec3(0, tileRaiseAmount, 0));
				RecursiveSetHighlightColour(hoveredGameObject, HexGameInfo::highlightColour);
			}

			selectedGameObject = hoveredGameObject;
		}
	}
}


void HexGame::Initialise(uint* renderTargetInit)
{
	AppInfo::name = "Cosy Hex Game";

	const string defaultPath = "Engine\\DefaultAssets\\";
	const string fullScreenQuadShader = StringBuilder(defaultPath, "FullScreenQuad.vert").value;

	PostProcess::Defaults::Initialise();

	const string outlineShader = "Assets\\Shaders\\Outline.frag";
	postProcessStack.Add(
		PostProcess(
			DrawOutline, RefreshOutline,
			{ shaderManager->GetProgram("Assets\\Shaders\\Outline.gpu")},
			{ &handles.outlineFBO },
			{ &handles.hdrTexture, &handles.positionTexture, &handles.idTexture, &handles.outlinedTexture },
			"Outline"
		)
	);

	const string hdrToLDRShader = StringBuilder(defaultPath, "HDR.frag").value;
	postProcessStack.Add(
		PostProcess(
			PostProcess::Defaults::DrawHDRToLDR, PostProcess::Defaults::RefreshHDRToLDR,
			{ new ShaderProgram(fullScreenQuadShader.c_str(), hdrToLDRShader.c_str()) },
			{ &handles.hdrToLDRFBO },
			{ &handles.outlinedTexture, &handles.gizmosTexture, &handles.uiTexture, &handles.ldrTexture },
			"HDR To LDR"
		)
	);

	const string fxaaShader = StringBuilder(defaultPath, "FXAA.frag").value;
	postProcessStack.Add(
		PostProcess(
			PostProcess::Defaults::DrawFXAA, PostProcess::Defaults::RefreshFXAA,
			{ new ShaderProgram(fullScreenQuadShader.c_str(), fxaaShader.c_str()) },
			{ &handles.outputFBO },
			{ &handles.ldrTexture, renderTargetInit },
			"FXAA"
		)
	);


	LoadTileData();

	// Preload
	//for (std::vector<TileData>& tileType : TileData::tilesData)
	//{
	//	for (TileData& tileVariant : tileType)
	//	{
	//		TileData::GetPrefab(tileVariant.name);
	//	}
	//}


	hexGrid = GameObject::Instantiate<HexGrid>();

	AppInfo::gameCamera->SetPosition(vec3(0, HexCameraData::offsetDirection.x, HexCameraData::offsetDirection.y) * HexCameraData::startZoom);
	AppInfo::gameCamera->LookAt(vec3(0, 0, 0));
	AppInfo::gameCamera->fov = glm::radians(50.0f);

	//HexFog::MakeHexagonalDistanceField();
	HexFog::Load(HexFog::GetFilter());
}
void HexGame::OnClose()
{
	SaveTileData();
}

void HexGame::OnStart()
{
	HexScrapbook::CacheEnabledStatus();

	HexScrapbook::Close();

	selectedPosition = vec2(0, 0);
	selectedGameObject = nullptr;
	AppInfo::gameCamera->SetPosition(vec3(0, HexCameraData::offsetDirection.x, HexCameraData::offsetDirection.y) * HexCameraData::startZoom);
	AppInfo::gameCamera->LookAt(vec3(0, 0, 0));
	HexCameraData::currentZoom = HexCameraData::startZoom;
	hexGrid->InitialiseCentre();

	treeRadial = new RadialMenu(HexRadial::treeRadialSprites[0].c_str(), HexRadial::treeRadialSprites[1].c_str(), HexRadial::treeRadialSprites[2].c_str());
	treeRadial->interactionHandler = { (RadialMenu::InteractFunc)RadialInteractionHandler };
	flowerRadial = new RadialMenu(HexRadial::flowerRadialSprites[0].c_str(), HexRadial::flowerRadialSprites[1].c_str(), HexRadial::flowerRadialSprites[2].c_str());
	flowerRadial->interactionHandler = { (RadialMenu::InteractFunc)RadialInteractionHandler };
	waterRadial = new RadialMenu(HexRadial::waterRadialSprites[0].c_str(), HexRadial::waterRadialSprites[1].c_str(), HexRadial::waterRadialSprites[2].c_str());
	waterRadial->interactionHandler = { (RadialMenu::InteractFunc)RadialInteractionHandler };
	landRadial = new RadialMenu(HexRadial::landRadialSprites[0].c_str(), HexRadial::landRadialSprites[1].c_str(), HexRadial::landRadialSprites[2].c_str());
	landRadial->interactionHandler = { (RadialMenu::InteractFunc)RadialInteractionHandler };

	currentRadialPage = 0U;
	currentTileType = &TileData::Get(HexType::Tree);
	currentTileVariant = 0U;

	crosshair = new UISprite("Assets\\UI\\Crosshair\\Crosshair.png", vec2(0.0f, 0.0f), vec2(0.0f, 0.0f), -10, 0.035f);

	if (!HexGameInfo::backgroundPrefab.empty())
	{
		HexGameInfo::background = (GameObject3D*)GameObject::InstantiateFrom(HexGameInfo::backgroundPrefab, GuidGeneration::New);
	}

	HexProgression::Initialise();

	HexScrapbook::ConcealSprites();

	HexAudio::BeginMusic();
	HexAudio::BeginAmbience();
}
void HexGame::OnStop()
{
	HexScrapbook::RestoreEnabledStatus();

	SetState(HexGame::State::Place);

	short min = hexGrid->centre.x - HexProgression::currentRadius;
	short max = hexGrid->centre.x + HexProgression::currentRadius;

	for (short c = min; c <= max; c++)
	{
		for (short r = min; r <= max; r++)
		{
			HexTile& hexTile = hexGrid->Get(HexOffsetCoord(c, r));

			GameObject::Destroy(hexTile.object);
			hexTile.object = nullptr;
			hexTile.habitat = -1;
			hexTile.type = HexType::Unreached;
			hexTile.variant = 0;
		}
	}

	
	for (Habitat& habitat : hexGrid->habitats)
	{
		GameObject::Destroy(habitat.object);
	}
	hexGrid->habitats.clear();

	HexProgression::ResetProgression();

	del(treeRadial);
	del(flowerRadial);
	del(waterRadial);
	del(landRadial);

	del(crosshair);

	GameObject::Destroy(HexGameInfo::background);

	TileData::ClearAndReset();
	HabitatData::ClearAndReset();
	AudioManager::EndAll();
	AudioManager::ClearLoadedAssets();
	HexScrapbook::RevealSprites();
	HexScrapbook::Reset();

	gameObjectManager->ClearGraveyard();
}

void HexGame::OnPause() { }
void HexGame::OnUnpause() { }

void HexGame::FixedUpdate() { }
void HexGame::Update()
{
	for (Spirit spirit : hexGrid->spirits)
	{
		if (spirit.object == nullptr) continue;

		vec3 pos = spirit.object->GetPosition();

		const float speed = 0.75f;
		const float amount = 0.1f;
		pos.y = 0.5f * amount + sin(Time::time * speed) * amount;

		if (spirit.habitatID == 2) pos.y *= 100.0f; // Temporary to account for the frog habitat being scaled
		
		spirit.object->SetPosition(pos);
	}

	RadialMenu* currentRadialMenu;
	vector<TileData>* currentRadialTileType;
	switch (currentRadialPage)
	{
	default: currentRadialMenu = treeRadial;		break;
	case 1U: currentRadialMenu = flowerRadial;	break;
	case 2U: currentRadialMenu = waterRadial;		break;
	case 3U: currentRadialMenu = landRadial;		break;
	}
	currentRadialTileType = &TileData::Get(HexType(currentRadialPage));

	switch (gameState)
	{
	case HexGame::State::Place:
	{
		if (gameInputs.place.pressed())
		{
			const json* prefab = TileData::GetPrefab((*currentTileType)[currentTileVariant].name);
			if (prefab == nullptr)
			{
				Debug::LogWarning("Tile ", (*currentTileType)[currentTileVariant].name, " couldn't load a valid prefab", locationinfo);
			}
			else
			{
				HexGrid::UpdateTileReturnInfo returnInfo = hexGrid->UpdateTile(vec3(selectedPosition.x, 0, selectedPosition.y), *prefab);
				if (returnInfo.value & HexGrid::UpdateTileReturnInfo::HabitatPickedUp)
				{
					//SetState(State::MoveHabitat);
					// TODO: Implement moving the habitat
				}
				else
				{
					if (returnInfo.value & HexGrid::UpdateTileReturnInfo::NewHabitat)
					{
						HexAudio::PlayMiscSFX(HexAudio::SoundEffect::FormHabitat);
						currentEvent = HexProgression::PlayHabitatStickerAnimation(returnInfo.habitatID);
						SetState(State::StickerEvent);
					}
					if (returnInfo.value & HexGrid::UpdateTileReturnInfo::TilePlaced)
					{
						selectedGameObject = nullptr;
						RaiseTile(selectedPosition, selectedGameObject, hexGrid);
					}

					if (returnInfo.value & HexGrid::UpdateTileReturnInfo::MilestoneReached)
					{
						//HexAudio::PlayMiscSFX(HexAudio::SoundEffect::MilestoneReached);
						// TODO: Play MilestoneReached SFX
					}
				}
			}
		}

		if (gameInputs.openRadial.pressed()) SetState(HexGame::State::Radial);
		else if (gameInputs.openScrapbook.pressed()) SetState(HexGame::State::Scrapbook);
		break;
	}
	case HexGame::State::Radial:
	{
		if (gameInputs.radialPageLeft.pressed())
		{
			HexAudio::PlayMiscSFX(HexAudio::SoundEffect::RadialCyclePage);
			currentRadialMenu->enabled = false;

			if (currentRadialPage == 0) currentRadialPage = 3U;
			else currentRadialPage = currentRadialPage - 1U;

			switch (currentRadialPage)
			{
			default: currentRadialMenu = treeRadial;		break;
			case 1U: currentRadialMenu = flowerRadial;	break;
			case 2U: currentRadialMenu = waterRadial;		break;
			case 3U: currentRadialMenu = landRadial;		break;
			}
			currentRadialTileType = &TileData::Get(HexType(currentRadialPage));
			currentRadialMenu->enabled = true;
		}
		if (gameInputs.radialPageRight.pressed())
		{
			HexAudio::PlayMiscSFX(HexAudio::SoundEffect::RadialCyclePage);
			currentRadialMenu->enabled = false;

			if (currentRadialPage == 3U) currentRadialPage = 0U;
			else currentRadialPage = currentRadialPage + 1U;

			switch (currentRadialPage)
			{
			default: currentRadialMenu = treeRadial;		break;
			case 1U: currentRadialMenu = flowerRadial;	break;
			case 2U: currentRadialMenu = waterRadial;		break;
			case 3U: currentRadialMenu = landRadial;		break;
			}
			currentRadialTileType = &TileData::Get(HexType(currentRadialPage));
			currentRadialMenu->enabled = true;
		}
		if (gameInputs.radialClose.pressed()) SetState(HexGame::State::Place);

		for (ushort i = 0; i < (ushort)currentTileType->size(); i++)
		{
			currentRadialMenu->sliceEnabledFlags[i] = (*currentRadialTileType)[i].unlocked;
		}
		break;
	}
	case HexGame::State::Scrapbook:
	{
		if (gameInputs.scrapbookClose.pressed()) SetState(HexGame::State::Place);
		break;
	}
	case HexGame::State::StickerEvent:
	{
		if (currentEvent->function == nullptr)
		{
			SetState(State::Place);
		}
	}
	}

	vec2 radialDirection = vec2(gameInputs.radialX, -gameInputs.radialY);
	
	float aspect = 1.0f / (AppInfo::screenHeight == 0 ? 0.0f : (AppInfo::screenWidth / (float)AppInfo::screenHeight));
	currentRadialMenu->Update(vec2(radialDirection.x / aspect, radialDirection.y), gameInputs.radialSelect);
	
	float xMovement = 0.0f;
	float zMovement = 0.0f;

	if (gameState == State::Place || gameState == State::Scrapbook)
	{
		xMovement = gameInputs.moveX;
		zMovement = gameInputs.moveZ;

		HexCameraData::currentZoom += (gameInputs.zoom - gameInputs.zoomIn + gameInputs.zoomOut) * HexCameraData::zoomSpeed * Time::delta;
		HexCameraData::currentZoom = glm::clamp(HexCameraData::currentZoom, HexCameraData::minZoom, HexCameraData::maxZoom);
	}
	crosshair->enabled = !currentRadialMenu->enabled;

	vec2 moveDir;
	if (glm::length(vec2(xMovement, zMovement)) > 1)
	{
		moveDir = glm::normalize(vec2(xMovement, zMovement));
	}
	else
	{
		moveDir = vec2(xMovement, zMovement);
	}

	selectedPosition += moveDir * moveSpeed * Time::delta;
	vec3 offset3D = vec3(0, HexCameraData::offsetDirection) * HexCameraData::currentZoom;
	AppInfo::gameCamera->SetPosition(vec3(selectedPosition.x, 0, selectedPosition.y) + offset3D);

	RaiseTile(selectedPosition, selectedGameObject, hexGrid);
}
void HexGame::Draw()
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);

	// Create Shadowmaps
	LightingManager::DrawToShadowMaps(
		LightingManager::shadowMapFBO,
		LightingManager::shadowMaps,
		LightingManager::shadowMapCount,
		LightingManager::maxShadowMaps,
		LightingManager::shadowMapSideLength,
		LightingManager::lightObjects);

	// Scene Drawing
	GameObjectManager::DrawScene(
		handles.hdrFBO,
		handles.hdrTexture,
		handles.hdrDepth,
		handles.positionTexture,
		handles.idTexture,
		handles.gizmosTexture,
		transformsDrawFunc);

	// UI Drawing
	if (displayUI)
	{
		RadialMenu* currentRadialMenu;
		switch (currentRadialPage)
		{
		default: currentRadialMenu = treeRadial;		break;
		case 1U: currentRadialMenu = flowerRadial;	break;
		case 2U: currentRadialMenu = waterRadial;		break;
		case 3U: currentRadialMenu = landRadial;		break;
		}

		DrawUI(
			currentRadialMenu,
			{/*crosshair*/},
			handles.uiFBO,
			handles.uiTexture);
	}

	/// Screen Post Processing
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	PostProcess::Defaults::displayUI = displayUI;
	if (AppInfo::screenSizeJustChanged) { postProcessStack.RefreshForScreenSize(); }

	postProcessStack.Draw();
}

void HexGame::LoadTileData() noexcept
{
	const char* const dataPath = "Assets\\HexGameData.json";
	ifstream input(dataPath);
	json dataJSON;

	if (!input.good())
	{
		Debug::Log("TileData.json was not found and has been created.");
		SaveTileData();
		return;
	}

	try { input >> dataJSON; }
	catch (parse_error)
	{
		Debug::LogWarning("TileData.json was found to be corrupt, it has been recreated.");
		HexGameSerialiser::SaveDataTo(dataJSON);
		ofstream output(dataPath);
		output << std::setw(2) << dataJSON;
		return;
	}

	HexGameSerialiser::LoadDataFrom(dataJSON);
}

void HexGame::SaveTileData() noexcept
{
	const char* const dataPath = "Assets\\HexGameData.json";

	json dataJSON;
	HexGameSerialiser::SaveDataTo(dataJSON);

	ofstream output(dataPath);
	output << std::setw(2) << dataJSON;
}
