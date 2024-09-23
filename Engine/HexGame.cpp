#include "HexGame.h"
#include "AppInfo.h"

#include "LightingManager.h"
#include "GameObjectManager.h"
#include "AudioManager.h"

#include "Shader.h"
#include "Camera.h"

#include "StringBuilder.h"

#include "HexProgression.h"
#include "HexAudio.h"
#include "HexRadial.h"

#include "HabitatData.h"

#include "HexGameSerialiser.h"

#include "RandomGen.h"

#include "Debug.h"

void static RadialInteractionHandler(uint selection)
{
	currentTileType = &TileData::Get(HexType(currentRadialPage));
	currentTileVariant = selection;
}

void static RefreshHexPositions(uint& hexPosFBO, uint& hexPosTexture, uint& hexPosDepth)
{
	if (hexPosFBO == 0)
	{
		glGenFramebuffers(1, &hexPosFBO);
		glGenRenderbuffers(1, &hexPosDepth);
		glGenTextures(1, &hexPosTexture);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, hexPosFBO);

	glBindRenderbuffer(GL_RENDERBUFFER, hexPosDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, AppInfo::screenWidth, AppInfo::screenHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, hexPosDepth);

	glBindTexture(GL_TEXTURE_2D, hexPosTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32I, AppInfo::screenWidth, AppInfo::screenHeight, 0, GL_RGBA_INTEGER, GL_INT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hexPosTexture, 0);
}
void static DrawHexPositions(LinkedHexGrid* hexGrid, uint& hexPosFBO, uint& hexPosTexture, uint &hexPosDepth)
{
	if (hexPosFBO == 0 || AppInfo::screenSizeJustChanged)
	{
		RefreshHexPositions(hexPosFBO, hexPosTexture, hexPosDepth);
	}

	if (AppInfo::CompareState(AppState::Playing))
	{
		glBindFramebuffer(GL_FRAMEBUFFER, hexPosFBO);

		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, AppInfo::screenWidth, AppInfo::screenHeight);

		short min = hexGrid->centre.x - HexProgression::currentRadius;
		short max = hexGrid->centre.x + HexProgression::currentRadius;

		for (short c = min; c <= max; c++)
		{
			for (short r = min; r <= max; r++)
			{
				HexTile& hexTile = hexGrid->Get(HexOffsetCoord(c, r));

				if (hexTile.object != nullptr)
				{
					hexTile.DrawHexPos(hexGrid->centre);
				}
			}
		}
	}
}

static void RefreshUI(uint& uiFBO, uint& uiTexture, uint& uiDepth)
{
	if (uiFBO == 0)
	{
		glGenFramebuffers(1, &uiFBO);
		glGenTextures(1, &uiTexture);
		glGenRenderbuffers(1, &uiDepth);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, uiFBO);

	glBindTexture(GL_TEXTURE_2D, uiTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, AppInfo::screenWidth, AppInfo::screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindRenderbuffer(GL_RENDERBUFFER, uiDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, AppInfo::screenWidth, AppInfo::screenHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, uiDepth);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, uiTexture, 0);
}
static void DrawUI(RadialMenu* radialMenu, vector<UISprite*> sprites, uint& uiFBO, uint& uiTexture, uint& uiDepth)
{
	if (uiFBO == 0 || AppInfo::screenSizeJustChanged)
	{
		RefreshUI(uiFBO, uiTexture, uiDepth);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, uiFBO);
	
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (uint i = 0; i < (uint)sprites.size(); i++)
	{
		sprites[i]->Draw();
	}

	radialMenu->Draw();

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

	AppInfo::gameCamera->SetPosition(vec3(0, 4, 2));
	AppInfo::gameCamera->LookAt(vec3(0, 0, 0));
}
void HexGame::OnClose()
{
	SaveTileData();
}

void HexGame::OnStart()
{
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

	crosshair = new UISprite("Assets\\UI\\Crosshair\\Crosshair.png", vec2(0.0f, 0.0f), 0.035f);

	HexProgression::Initialise();

	HexAudio::BeginMusic();
	HexAudio::BeginAmbience();
}
void HexGame::OnStop()
{
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

	crosshair->normalisedPosition = vec2(cos(Time::time), sin(Time::time * 2.0f));

	if (currentRadialMenu->enabled)
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
		if (gameInputs.radialClose.pressed())
		{
			HexAudio::PlayMiscSFX(HexAudio::SoundEffect::RadialClose);
			currentRadialMenu->enabled = false;
		}

		for (ushort i = 0; i < (ushort)currentTileType->size(); i++)
		{
			currentRadialMenu->sliceEnabledFlags[i] = (*currentRadialTileType)[i].unlocked;
		}
	}
	else
	{
		if (gameInputs.place.pressed())
		{
			// TODO: replace this with a raycast for performance reasons
			const int pixelCount = AppInfo::screenWidth * AppInfo::screenHeight;
			glm::ivec4* hexPosPixels = new glm::ivec4[pixelCount];
			glGetTextureImage(handles.hexPosTexture, 0, GL_RGBA_INTEGER, GL_INT, pixelCount * sizeof(glm::ivec4), hexPosPixels);

			uint hexPosIndex = (int)AppInfo::screenWidth/2 + (int)AppInfo::screenHeight/2 * AppInfo::screenWidth;
			glm::ivec4 hexPos = hexPosPixels[hexPosIndex];
			if (hexPos.a != 0)
			{
				hexGrid->UpdateTile(HexOffsetCoord(hexPos.x, hexPos.y), TileData::GetPrefab((*currentTileType)[currentTileVariant].name));
			}
		}

		if (gameInputs.openRadial.pressed())
		{
			HexAudio::PlayMiscSFX(HexAudio::SoundEffect::RadialOpen);
			currentRadialMenu->enabled = true;
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

	vec3 moveStep = vec3(moveDir.x, 0, moveDir.y) * moveSpeed * Time::delta;
	AppInfo::gameCamera->Translate(moveStep);
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

	// Hex Pos Drawing
	DrawHexPositions(
		hexGrid,
		handles.hexPosFBO,
		handles.hexPosTexture,
		handles.hexPosDepth
	);

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
			handles.uiTexture,
			handles.uiDepth);
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
