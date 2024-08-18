#include "HexGame.h"
#include "AppInfo.h"

#include "LightingManager.h"
#include "GameObjectManager.h"

#include "Shader.h"
#include "Camera.h"

#include "StringBuilder.h"

void static RadialInteractionHandler(uint selection)
{
	currentTileType = (HexType)(selection + 1);
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

	if (AppInfo::state == AppState::Playing)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, hexPosFBO);

		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, AppInfo::screenWidth, AppInfo::screenHeight);

		for (HexPair hexPair : hexGrid->lookupTable)
		{
			hexPair.second->DrawHexPos();
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
static void DrawUI(RadialMenu* radialMenu, uint& uiFBO, uint& uiTexture, uint& uiDepth)
{
	if (uiFBO == 0 || AppInfo::screenSizeJustChanged)
	{
		RefreshUI(uiFBO, uiTexture, uiDepth);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, uiFBO);
	
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

	hexGrid = GameObject::Instantiate<LinkedHexGrid>();

	hexGrid->AddTile(vec3(0, 0, 0), HexDir::NorthWest);
	hexGrid->AddTile(vec3(0, 0, 0), HexDir::North);
	hexGrid->AddTile(vec3(0, 0, 0), HexDir::NorthEast);
	hexGrid->AddTile(vec3(0, 0, 0), HexDir::SouthEast);
	hexGrid->AddTile(vec3(0, 0, 0), HexDir::South);
	hexGrid->AddTile(vec3(0, 0, 0), HexDir::SouthWest);

	radialMenu = new RadialMenu();
	radialMenu->interactionHandler = { (RadialMenu::InteractFunc)RadialInteractionHandler };

	AppInfo::gameCamera->SetPosition(vec3(0, 4, 2));
	AppInfo::gameCamera->LookAt(vec3(0, 0, 0));
}
void HexGame::OnClose()
{
	del(radialMenu);
}

void HexGame::OnStart() { }
void HexGame::OnStop() { }

void HexGame::OnPause() { }
void HexGame::OnUnpause() { }

void HexGame::FixedUpdate() { }
void HexGame::Update()
{
	if (AppInfo::input->screenCursorPos.x >= 0 &&
		AppInfo::input->screenCursorPos.y >= 0 &&
		AppInfo::input->screenCursorPos.x < AppInfo::screenWidth &&
		AppInfo::input->screenCursorPos.y < AppInfo::screenHeight)
	{
		if (!radialMenu->enabled && AppInfo::input->GetKeyPressed(KeyCode::MouseLeft))
		{
			const int pixelCount = AppInfo::screenWidth * AppInfo::screenHeight;
			glm::ivec4* hexPosPixels = new glm::ivec4[pixelCount];
			glGetTextureImage(handles.hexPosTexture, 0, GL_RGBA_INTEGER, GL_INT, pixelCount * sizeof(glm::ivec4), hexPosPixels);
			uint hexPosIndex = (int)AppInfo::input->screenCursorPos.x + (int)AppInfo::input->screenCursorPos.y * AppInfo::screenWidth;
			glm::ivec4 hexPos = hexPosPixels[hexPosIndex];
			if (hexPos.a != 0)
			{
				hexGrid->UpdateTile((glm::ivec2)hexPos, currentTileType);
			}
		}

		if (AppInfo::input->GetKeyPressed(KeyCode::Space)) radialMenu->enabled = true;

		vec2 cursorNormalised = AppInfo::input->screenCursorPos / vec2(AppInfo::screenWidth, AppInfo::screenHeight) * 2.0f - 1.0f;
		float aspect = 1.0f / (AppInfo::screenHeight == 0 ? 0.0f : (AppInfo::screenWidth / (float)AppInfo::screenHeight));
		radialMenu->Update(vec2(cursorNormalised.x / aspect, cursorNormalised.y), Keybind(KeyCode::MouseLeft));
	}

	KeyAxis xMovement(KeyCode::A, KeyCode::D);
	KeyAxis zMovement(KeyCode::W, KeyCode::S);
	vec2 moveDir = xMovement == 0.0f && zMovement == 0.0f ?
		vec2(0.0f) :
		glm::normalize(vec2((float)xMovement, (float)zMovement));

	vec3 moveStep = vec3(moveDir.x, 0, moveDir.y) * 3.0f * Time::delta;
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
		DrawUI(
			radialMenu,
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
