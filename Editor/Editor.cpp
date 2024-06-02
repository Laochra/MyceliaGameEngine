#include "Editor.h"

#include "ImGuiIncludes.h"

#include "EditorCamera.h"

#include "EditorGUI.h"
#include "PostProcessingGUI.h"

#include "GeneralMacros.h"

void Editor::Initialise()
{
	EditorGUI::Initialise();
	glfwSetDropCallback(window, EditorGUI::FileDropCallback);

	gameObjectManager = new GameObjectManager();
	
	mainCamera = GameObject3D::Instantiate<EditorCamera>(vec3(0.0f, 10.0f, 10.0f));
	mainCamera->SetPosition({ -0.25, 1.0f, 1.5f });
	((EditorCamera*)mainCamera)->xRotation = -80;
	((EditorCamera*)mainCamera)->yRotation = -20;

	LightingManager::ambientLight = Light(vec3(0.1f, 0.1f, 0.1f));
	LightingManager::directionalLight = DirectionalLight(vec3(1.0f, 1.0f, 1.0f), glm::normalize(vec3(-0.1f, -1, -1)));

	debug->lines.AddXYZLines(-FLT_MAX);
	
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
}
void Editor::Draw()
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glDeleteFramebuffers(1, &EditorGUI::sceneViewFrameBufferHDR);
	glDeleteTextures(2, &EditorGUI::sceneViewColourBufferHDR[0]);
	glDeleteRenderbuffers(1, &EditorGUI::sceneViewDepthStencilBuffer);

	glGenFramebuffers(1, &EditorGUI::sceneViewFrameBufferHDR);
	glBindFramebuffer(GL_FRAMEBUFFER, EditorGUI::sceneViewFrameBufferHDR);

	glGenRenderbuffers(1, &EditorGUI::sceneViewDepthStencilBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, EditorGUI::sceneViewDepthStencilBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, EditorGUI::sceneViewDepthStencilBuffer);

	// Main Colour Textures
	glGenTextures(2, &EditorGUI::sceneViewColourBufferHDR[0]);
	for (uint texId = 0; texId < 2; texId++)
	{
		glBindTexture(GL_TEXTURE_2D, EditorGUI::sceneViewColourBufferHDR[texId]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + texId, GL_TEXTURE_2D, EditorGUI::sceneViewColourBufferHDR[texId], 0);
	}
	uint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);

	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, screenWidth, screenHeight);

	Updater::CallDraw();


	// Draw Debug Gizmos Unaffected by Post Processing
	glDeleteTextures(1, &EditorGUI::sceneViewColourBufferGizmos);
	glGenTextures(1, &EditorGUI::sceneViewColourBufferGizmos);
	glBindTexture(GL_TEXTURE_2D, EditorGUI::sceneViewColourBufferGizmos);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, EditorGUI::sceneViewColourBufferGizmos, 0);
	uint attachment[1] = { GL_COLOR_ATTACHMENT0 };

	glDrawBuffers(1, attachment);

	glEnable(GL_DEPTH_TEST);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	vec3 cameraPos(mainCamera->GetPosition());
	debug->lines.AddGrid(vec3((int)cameraPos.x, 0, (int)cameraPos.z), 50);

	debug->lines.Draw();

	/// Screen Post Processing
	glDisable(GL_CULL_FACE);
	Mesh screenQuad;
	screenQuad.InitialiseQuad();

	// Bloom Passes
	const int downResFactor = 4;
	glViewport(0, 0, screenWidth / downResFactor, screenHeight / downResFactor);
	uint blurFrameBuffers[2]{};
	uint blurColourBuffers[2]{};
	glGenFramebuffers(2, &blurFrameBuffers[0]);
	glGenTextures(2, &blurColourBuffers[0]);

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

	glDisable(GL_DEPTH_TEST);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	int passes = 4;
	blurProgram.Bind();
	blurProgram.BindUniform("BrightTexture", 0);
	for (uint p = 0; p < (uint)passes; p++)
	{
		bool b = p % 2;
		glActiveTexture(GL_TEXTURE0);
		glBindFramebuffer(GL_FRAMEBUFFER, blurFrameBuffers[b]);
		glBindTexture(GL_TEXTURE_2D, p == 0 ? EditorGUI::sceneViewColourBufferHDR[1] : blurColourBuffers[!b]);

		screenQuad.Draw();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, screenWidth, screenHeight);


	// Normalising HDR for Screen
	unsigned int ldrFrameBuffer;
	unsigned int ldrColourBuffer;
	glGenFramebuffers(1, &ldrFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, ldrFrameBuffer);

	glGenTextures(1, &ldrColourBuffer);
	glBindTexture(GL_TEXTURE_2D, ldrColourBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ldrColourBuffer, 0);

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

	glDeleteFramebuffers(2, &blurFrameBuffers[0]);
	glDeleteTextures(2, &blurColourBuffers[0]);

	// FXAA
	glDeleteFramebuffers(1, &EditorGUI::sceneViewFrameBufferOutput);
	glDeleteTextures(1, &EditorGUI::sceneViewColourBufferOutput);

	glGenFramebuffers(1, &EditorGUI::sceneViewFrameBufferOutput);
	glBindFramebuffer(GL_FRAMEBUFFER, EditorGUI::sceneViewFrameBufferOutput);

	glGenTextures(1, &EditorGUI::sceneViewColourBufferOutput);
	glBindTexture(GL_TEXTURE_2D, EditorGUI::sceneViewColourBufferOutput);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, EditorGUI::sceneViewColourBufferOutput, 0);

	fxaaProgram.Bind();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ldrColourBuffer);
	fxaaProgram.BindUniform("AliasedTexture", 0);

	glClear(GL_COLOR_BUFFER_BIT);

	screenQuad.Draw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDeleteFramebuffers(1, &ldrFrameBuffer);
	glDeleteTextures(1, &ldrColourBuffer);
	

	EditorGUI::RenderGUI();
}

bool Editor::OnClose()
{
	if (!EditorGUI::CleanUp()) return false;

	del(mainCamera);

	del(gameObjectManager);

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	return true;
}
