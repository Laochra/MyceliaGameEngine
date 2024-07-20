#include "Editor.h"

#include "ImGuiIncludes.h"

#include "EditorCamera.h"

#include "EditorGUI.h"
#include "PostProcessingGUI.h"

#include "GeneralMacros.h"

#include "ParticleEmitter.h"

#include "TransformEdit.h"

void Editor::Initialise()
{
	EditorGUI::Initialise();
	glfwSetDropCallback(window, EditorGUI::FileDropCallback);

	gameObjectManager = new GameObjectManager();
	
	Camera::main = GameObject3D::Instantiate<EditorCamera>(vec3(-0.25f, 1.0f, 1.5f));
	EditorCamera::main()->xRotation = -80;
	EditorCamera::main()->yRotation = -20;

	LightingManager::ambientLight = Light(vec3(0.1f, 0.1f, 0.1f));
	LightingManager::directionalLight = DirectionalLight(vec3(1.0f, 1.0f, 1.0f), glm::normalize(vec3(-0.1f, -1, -1)));
	
	input->enabled = false;

	// Initialise Post Processing Shaders // TODO: Move this into its own place
	{
		hdrProgram.LoadShader(VertexStage, "Engine\\DefaultAssets\\FullScreenQuad.vert");
		hdrProgram.LoadShader(FragmentStage, "Engine\\DefaultAssets\\HDR.frag");
		hdrProgram.Link();

		blurProgram.LoadShader(VertexStage, "Engine\\DefaultAssets\\FullScreenQuad.vert");
		blurProgram.LoadShader(FragmentStage, "Engine\\DefaultAssets\\BoxBlur.frag");
		blurProgram.Link();

		fxaaProgram.LoadShader(VertexStage, "Engine\\DefaultAssets\\FullScreenQuad.vert");
		fxaaProgram.LoadShader(FragmentStage, "Engine\\DefaultAssets\\FXAA.frag");
		fxaaProgram.Link();
	}
}

void Editor::FixedUpdate()
{
	Updater::CallFixedUpdate();
}
void Editor::Update()
{	
	EditorGUI::Draw();
	Updater::CallUpdate();
	TransformEdit::Update();
}
void Editor::Draw()
{
	if (screenWidth <= 0 || screenHeight <= 0)
	{
		EditorGUI::RenderGUI();
		return;
	}

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glViewport(0, 0, screenWidth, screenHeight);

	// Generate GUID Texture
	if (EditorGUI::guidFBO == 0) glGenFramebuffers(1, &EditorGUI::guidFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, EditorGUI::guidFBO);

	if (EditorGUI::guidDepth == 0) glGenRenderbuffers(1, &EditorGUI::guidDepth);
	if (screenSizeJustChanged)
	{
		glBindRenderbuffer(GL_RENDERBUFFER, EditorGUI::guidDepth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, EditorGUI::guidDepth);
	}

	if (EditorGUI::guidTexture == 0) glGenTextures(1, &EditorGUI::guidTexture);
	if (screenSizeJustChanged)
	{
		glBindTexture(GL_TEXTURE_2D, EditorGUI::guidTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32UI, screenWidth, screenHeight, 0, GL_RG_INTEGER, GL_UNSIGNED_INT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, EditorGUI::guidTexture, 0);
	}

	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	Updater::CallDrawGUIDs();
	glClear(GL_DEPTH_BUFFER_BIT);
	TransformEdit::DrawIDs();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Create Shadowmaps
	glCullFace(GL_FRONT);
	uint& shadowMapFBO = LightingManager::shadowMapFBO;
	uint& shadowMaps = LightingManager::shadowMaps;
	uint& shadowSideLength = LightingManager::shadowMapSideLength;

	if (shadowMapFBO == 0)
	{
		glGenFramebuffers(1, &shadowMapFBO);
		glGenTextures(1, &shadowMaps);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
		glBindTexture(GL_TEXTURE_2D_ARRAY, shadowMaps);
		glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_DEPTH24_STENCIL8, shadowSideLength, shadowSideLength, LightingManager::maxShadowMaps);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glBindTexture(GL_TEXTURE_2D_ARRAY, shadowMaps);
	uint& mapCount = LightingManager::shadowMapCount;
	mapCount = 0U;
	for (uint l = 0; l < (uint)LightingManager::lightObjects.size(); l++)
	{
		mapCount += LightingManager::lightObjects[l]->shadowMapCount;
	}
	
	if (mapCount > 0)
	{
		if (mapCount > LightingManager::maxShadowMaps)
		{
			Debug::LogWarning("There are more shadow maps (", mapCount, ") "
				"than are allowed (", LightingManager::maxShadowMaps, "). "
				"Reduce the amount of shadow mapped lights.");
		}

		uint mapIndex = 0;
		for (uint l = 0; l < (uint)LightingManager::lightObjects.size() && l < LightingManager::maxShadowMaps; l++)
		{
			LightObject& lightObject = *LightingManager::lightObjects[l];
			if (lightObject.shadowMode == NoShadows) continue;

			vector<mat4> pvMatrices = lightObject.GetLightPVMatrices();

			for (uint m = 0; m < lightObject.shadowMapCount; m++)
			{
				lightObject.shadowMaps[m] = mapIndex;

				glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
				glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, LightingManager::shadowMaps, 0, mapIndex);
				glDrawBuffer(GL_NONE);
				glReadBuffer(GL_NONE);
				glBindFramebuffer(GL_FRAMEBUFFER, 0);

				glViewport(0, 0, shadowSideLength, shadowSideLength);
				glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
				glClear(GL_DEPTH_BUFFER_BIT);

				Updater::CallDrawDepth(pvMatrices[m]);

				glBindFramebuffer(GL_FRAMEBUFFER, 0);

				mapIndex++;
			}
		}
	}

	glCullFace(GL_BACK);

	// Scene Drawing Begins
	if (EditorGUI::sceneViewFrameBufferHDR == 0)
	{
		glGenFramebuffers(1, &EditorGUI::sceneViewFrameBufferHDR);
		glGenRenderbuffers(1, &EditorGUI::sceneViewDepthStencilBuffer);
		glGenTextures(2, &EditorGUI::sceneViewColourBufferHDR[0]);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, EditorGUI::sceneViewFrameBufferHDR);

	if (screenSizeJustChanged)
	{
		glBindRenderbuffer(GL_RENDERBUFFER, EditorGUI::sceneViewDepthStencilBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, EditorGUI::sceneViewDepthStencilBuffer);
		
		for (uint texId = 0; texId < 2; texId++)
		{
			glBindTexture(GL_TEXTURE_2D, EditorGUI::sceneViewColourBufferHDR[texId]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, EditorGUI::sceneViewColourBufferHDR[0], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, EditorGUI::sceneViewColourBufferHDR[1], 0);
	uint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);

	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, screenWidth, screenHeight);

	Updater::CallDraw();


	// Draw Debug Gizmos Unaffected by Post Processing
	if (EditorGUI::sceneViewColourBufferGizmos == 0)
	{
		glGenTextures(1, &EditorGUI::sceneViewColourBufferGizmos);
	}
	if (screenSizeJustChanged)
	{
		glBindTexture(GL_TEXTURE_2D, EditorGUI::sceneViewColourBufferGizmos);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, EditorGUI::sceneViewColourBufferGizmos, 0);
	uint attachment[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, attachment);

	glEnable(GL_DEPTH_TEST);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	vec3 cameraPos(Camera::main->GetPosition());
	debug->lines.AddGrid(vec3((int)cameraPos.x, 0, (int)cameraPos.z), 50);

	debug->lines.Draw();

	glClear(GL_DEPTH_BUFFER_BIT);
	TransformEdit::Draw();

	/// Screen Post Processing
	glDisable(GL_CULL_FACE);
	Mesh screenQuad;
	screenQuad.InitialiseQuad();

	// Bloom Passes
	const int downResFactor = 4;
	glViewport(0, 0, screenWidth / downResFactor, screenHeight / downResFactor);
	static uint blurFrameBuffers[2]{0,0};
	static uint blurColourBuffers[2]{0,0};

	if (blurFrameBuffers[0] == 0)
	{
		glGenFramebuffers(2, &blurFrameBuffers[0]);
		glGenTextures(2, &blurColourBuffers[0]);
	}

	if (screenSizeJustChanged)
	{
		for (int i = 0; i < 2; i++)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, blurFrameBuffers[i]);
			glBindTexture(GL_TEXTURE_2D, blurColourBuffers[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenWidth / downResFactor, screenHeight / downResFactor, 0, GL_RGBA, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurColourBuffers[i], 0);
		}
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	int passes = 4;
	blurProgram.Bind();
	blurProgram.BindUniform("BrightTexture", 0);
	for (uint p = 0; p < (uint)passes; p++)
	{
		bool b = p % 2;
		glActiveTexture(GL_TEXTURE0);
		glBindFramebuffer(GL_FRAMEBUFFER, blurFrameBuffers[b]);
		glBindTexture(GL_TEXTURE_2D, p == 0 ? EditorGUI::sceneViewColourBufferHDR[1] : blurColourBuffers[!b]);

		glDisable(GL_DEPTH_TEST);
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT);

		screenQuad.Draw();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, screenWidth, screenHeight);


	// Normalising HDR for Screen
	static uint ldrFrameBuffer = 0;
	static uint ldrColourBuffer = 0;
	if (ldrFrameBuffer == 0)
	{
		glGenFramebuffers(1, &ldrFrameBuffer);
		glGenTextures(1, &ldrColourBuffer);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, ldrFrameBuffer);

	if (screenSizeJustChanged)
	{
		glBindTexture(GL_TEXTURE_2D, ldrColourBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ldrColourBuffer, 0);
	}

	glClearColor(0.8f, 0.75f, 0.85f, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	hdrProgram.Bind();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, EditorGUI::sceneViewColourBufferHDR[0]);
	hdrProgram.BindUniform("HDRTexture", 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, blurColourBuffers[passes % 2]);
	hdrProgram.BindUniform("BloomTexture", 1);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, EditorGUI::sceneViewColourBufferGizmos);
	hdrProgram.BindUniform("GizmosTexture", 2);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, ldrColourBuffer);
	hdrProgram.BindUniform("CurrentColourBuffer", 3);
	hdrProgram.BindUniform("Exposure", PostProcessingGUI::exposure);

	screenQuad.Draw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// FXAA
	if (EditorGUI::sceneViewFrameBufferOutput == 0)
	{
		glGenFramebuffers(1, &EditorGUI::sceneViewFrameBufferOutput);
		glGenTextures(1, &EditorGUI::sceneViewColourBufferOutput);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, EditorGUI::sceneViewFrameBufferOutput);

	if (screenSizeJustChanged)
	{
		glBindTexture(GL_TEXTURE_2D, EditorGUI::sceneViewColourBufferOutput);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, EditorGUI::sceneViewColourBufferOutput, 0);
	}

	fxaaProgram.Bind();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ldrColourBuffer);
	fxaaProgram.BindUniform("AliasedTexture", 0);

	glClear(GL_COLOR_BUFFER_BIT);

	screenQuad.Draw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	EditorGUI::RenderGUI();
}

bool Editor::OnClose()
{
	if (!EditorGUI::CleanUp()) return false;

	del(Camera::main);

	del(gameObjectManager);

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	return true;
}
