#include "ExampleGame.h"
#include "AppInfo.h"

#include "LightingManager.h"
#include "GameObjectManager.h"

#include "Shader.h"
#include "Camera.h"

#include "StringBuilder.h"

#include "AudioManager.h"
#include "Input.h"

void ExampleGame::Initialise(uint* renderTargetInit)
{
	const string defaultPath = "Engine\\DefaultAssets\\";
	const string fullScreenQuadShader = StringBuilder(defaultPath, "FullScreenQuad.vert").value;


	const string hdrToLDRShader = StringBuilder(defaultPath, "HDR.frag").value;
	postProcessStack.Add(
		PostProcess(
			PostProcess::Defaults::DrawHDRToLDR, PostProcess::Defaults::RefreshHDRToLDR,
			{ new ShaderProgram(fullScreenQuadShader.c_str(), hdrToLDRShader.c_str())},
			{ &handles.hdrToLDRFBO },
			{ &handles.hdrTexture, &handles.gizmosTexture, &handles.ldrTexture },
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

	AppInfo::name = "Example Game - Made in Mycelia Game Engine";
}
void ExampleGame::OnClose() { }

void ExampleGame::OnStart() { }
void ExampleGame::OnStop() { }

void ExampleGame::OnPause() { }
void ExampleGame::OnUnpause() { }

void ExampleGame::FixedUpdate() { }
void ExampleGame::Update() { }
void ExampleGame::Draw()
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

	// Scene Drawing Begins
	GameObjectManager::DrawScene(
		handles.hdrFBO,
		handles.hdrTexture,
		handles.hdrDepth,
		handles.positionTexture,
		handles.idTexture,
		handles.gizmosTexture,
		transformsDrawFunc);

	/// Screen Post Processing
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	if (AppInfo::screenSizeJustChanged) { postProcessStack.RefreshForScreenSize(); }

	postProcessStack.Draw();
}
