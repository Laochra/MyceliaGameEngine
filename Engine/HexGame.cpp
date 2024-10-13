#include "HexGame.h"
#include "AppInfo.h"

#include "LightingManager.h"
#include "GameObjectManager.h"
#include "AudioManager.h"
#include "UIManager.h"

#include "Shader.h"
#include "Camera.h"

#include "StringBuilder.h"

#include "HexProgression.h"
#include "HexAudio.h"
#include "HexRadial.h"
#include "HexScrapbook.h"

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
		HexScrapbook::SetEnabled(false);
		HexAudio::PlayMiscSFX(HexAudio::SoundEffect::ScrapbookClose);
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
		HexScrapbook::SetEnabled(true);
		HexAudio::PlayMiscSFX(HexAudio::SoundEffect::ScrapbookOpen);
		break;
	}
	default: Debug::LogWarning("Unaccounted for game state", locationinfo); break;
	}
}

void HexGame::Initialise(uint* renderTargetInit)
{
	AppInfo::name = "Cosy Hex Game";

	const string defaultPath = "Engine\\DefaultAssets\\";
	const string fullScreenQuadShader = StringBuilder(defaultPath, "FullScreenQuad.vert").value;

	PostProcess::Defaults::Initialise();

	const string boxBlurShader = StringBuilder(defaultPath, "BoxBlur.frag").value;
	postProcessStack.Add(
		PostProcess(
			PostProcess::Defaults::DrawBloom, PostProcess::Defaults::RefreshBloom,
			{ new ShaderProgram(fullScreenQuadShader.c_str(), boxBlurShader.c_str()) },
			{ &handles.bloomFBOs[0], &handles.bloomFBOs[1] },
			{ &handles.brightTexture, &handles.bloomTextures[0], &handles.bloomTextures[1] },
			"Bloom Blur"
		)
	);

	const string hdrToLDRShader = StringBuilder(defaultPath, "HDR.frag").value;
	postProcessStack.Add(
		PostProcess(
			PostProcess::Defaults::DrawHDRToLDR, PostProcess::Defaults::RefreshHDRToLDR,
			{ new ShaderProgram(fullScreenQuadShader.c_str(), hdrToLDRShader.c_str()) },
			{ &handles.hdrToLDRFBO },
			{ &handles.hdrTexture, &handles.bloomTextures[PostProcess::Defaults::bloomPasses % 2], &handles.gizmosTexture, &handles.uiTexture, &handles.ldrTexture },
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


	hexGrid = GameObject::Instantiate<LinkedHexGrid>();

	AppInfo::gameCamera->SetPosition(vec3(0, HexCameraData::offsetDirection.x, HexCameraData::offsetDirection.y));
	AppInfo::gameCamera->LookAt(vec3(0, 0, 0));
	AppInfo::gameCamera->fov = glm::radians(50.0f);
}
void HexGame::OnClose()
{
	SaveTileData();
}

void HexGame::OnStart()
{
	HexScrapbook::CacheEnabledStatus();

	HexScrapbook::SetEnabled(false);

	AppInfo::gameCamera->SetPosition(vec3(0, HexCameraData::offsetDirection.x, HexCameraData::offsetDirection.y));
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

	crosshair = new UISprite("Assets\\UI\\Crosshair\\Crosshair.png", vec2(0.0f, 0.0f), -10, 0.035f);

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

	TileData::ClearAndReset();
	HabitatData::ClearAndReset();
	AudioManager::EndAll();
	AudioManager::ClearLoadedAssets();
	HexScrapbook::RevealSprites();

	gameObjectManager->ClearGraveyard();
}

void HexGame::OnPause() { }
void HexGame::OnUnpause() { }

void HexGame::FixedUpdate() { }
void HexGame::Update()
{
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
			hexGrid->UpdateTile(vec3(selectedPosition.x, 0, selectedPosition.y), TileData::GetPrefab((*currentTileType)[currentTileVariant].name));
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
	}

	vec2 radialDirection = vec2(gameInputs.radialX, -gameInputs.radialY);
	
	float aspect = 1.0f / (AppInfo::screenHeight == 0 ? 0.0f : (AppInfo::screenWidth / (float)AppInfo::screenHeight));
	currentRadialMenu->Update(vec2(radialDirection.x / aspect, radialDirection.y), gameInputs.radialSelect);
	
	float xMovement = 0.0f;
	float zMovement = 0.0f;

	if (!currentRadialMenu->enabled)
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

	HexTile& hoveredHex = hexGrid->Get(HexOffsetCoord::GetFromPos(selectedPosition, hexGrid->centre));
	GameObject3D* hoveredGameObject;
	if (hoveredHex.habitat < 0)
	{
		hoveredGameObject = hoveredHex.object;
	}
	else
	{
		hoveredGameObject = nullptr;

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
			selectedGameObject->Translate(vec3(0, -0.1f, 0));
		}
		if (hoveredGameObject != nullptr)
		{
			hoveredGameObject->Translate(vec3(0, 0.1f, 0));
		}

		selectedGameObject = hoveredGameObject;
	}
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
		handles.brightTexture,
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
			{crosshair},
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
