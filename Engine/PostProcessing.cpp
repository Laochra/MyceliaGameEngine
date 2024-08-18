#include "PostProcessing.h"
#include "AppInfo.h"

#include "Camera.h"

#include "MeshManager.h"

vector<PostProcess> PostProcessStack::postProcesses;

PostProcess::PostProcess(
	PostProcess::Func drawFunction,
	PostProcess::Func screenSizeRefreshFunction,
	vector<ShaderProgram*> programsInit,
	vector<uint*> framebuffersInit,
	vector<uint*> texturesInit,
	string nameInit) noexcept :
	drawFunc(drawFunction),
	screenSizeRefreshFunc(screenSizeRefreshFunction),
	programs(programsInit),
	framebuffers(framebuffersInit),
	textures(texturesInit),
	usesScreenSize(screenSizeRefreshFunction != nullptr),
	name(nameInit){}
PostProcess::PostProcess(
	const PostProcess&& init) noexcept :
	drawFunc(init.drawFunc),
	screenSizeRefreshFunc(init.screenSizeRefreshFunc),
	programs(init.programs),
	framebuffers(init.framebuffers),
	textures(init.textures),
	usesScreenSize(init.usesScreenSize),
	name(init.name) {}

void PostProcess::Draw() noexcept
{
	drawFunc(programs, framebuffers, textures);
}
void PostProcess::RefreshForScreenSize() noexcept
{
	screenSizeRefreshFunc(programs, framebuffers, textures);
}

void PostProcessStack::Add(PostProcess&& postProcess)
{
	postProcesses.push_back(std::move(postProcess));
	stack.push_back(std::move(postProcess));
}

void PostProcessStack::Draw() noexcept
{
	for (PostProcess& postProcess : stack)
	{
		postProcess.Draw();
	}
}
void PostProcessStack::RefreshForScreenSize() noexcept
{
	for (PostProcess& postProcess : postProcesses)
	{
		postProcess.RefreshForScreenSize();
	}
}

// Default Post Processes
Mesh* PostProcess::Defaults::screenQuad;
void PostProcess::Defaults::Initialise()
{
	screenQuad = meshManager->GetMesh("ProceduralQuad");
}

void PostProcess::Defaults::RefreshBloom(
	vector<ShaderProgram*>& programs,
	vector<uint*>& framebuffers,
	vector<uint*>& textures) noexcept
{
	const ShaderProgram& bloomProgram = *programs[0];

	uint* pingPongFBOs[2] = { framebuffers[0], framebuffers[1] };

	const uint& inputTexture = *textures[0];
	uint* pingPongTextures[2] = {textures[1], textures[2]};

	if (*pingPongFBOs[0] == 0)
	{
		glGenFramebuffers(1, pingPongFBOs[0]);
		glGenFramebuffers(1, pingPongFBOs[1]);
		glGenTextures(1, pingPongTextures[0]);
		glGenTextures(1, pingPongTextures[1]);
	}

	for (int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, *pingPongFBOs[i]);
		glBindTexture(GL_TEXTURE_2D, *pingPongTextures[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, AppInfo::screenWidth / downResFactor, AppInfo::screenHeight / downResFactor, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *pingPongTextures[i], 0);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}
void PostProcess::Defaults::DrawBloom(
	vector<ShaderProgram*>& programs,
	vector<uint*>& framebuffers,
	vector<uint*>& textures) noexcept
{
	ShaderProgram& bloomProgram = *programs[0];

	uint* pingPongFBOs[2] = { framebuffers[0], framebuffers[1] };

	const uint& inputTexture = *textures[0];
	uint* pingPongTextures[2] = { textures[1], textures[2] };

	glViewport(0, 0, AppInfo::screenWidth / downResFactor, AppInfo::screenHeight / downResFactor);

	if (*pingPongFBOs[0] == 0)
	{
		glGenFramebuffers(1, pingPongFBOs[0]);
		glGenFramebuffers(1, pingPongFBOs[1]);
		glGenTextures(1, pingPongTextures[0]);
		glGenTextures(1, pingPongTextures[1]);
	}

	bloomProgram.Bind();
	bloomProgram.BindUniform("BrightTexture", 0);
	for (uint p = 0; p < (uint)bloomPasses; p++)
	{
		bool b = p % 2;
		glActiveTexture(GL_TEXTURE0);
		glBindFramebuffer(GL_FRAMEBUFFER, *pingPongFBOs[int(b)]);
		glBindTexture(GL_TEXTURE_2D, p == 0 ? inputTexture : *pingPongTextures[int(!b)]);

		glDisable(GL_DEPTH_TEST);
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT);

		screenQuad->Draw();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool PostProcess::Defaults::displayUI = false;
float PostProcess::Defaults::exposure = 3.0f;
void PostProcess::Defaults::RefreshHDRToLDR(
	vector<ShaderProgram*>& programs,
	vector<uint*>& framebuffers,
	vector<uint*>& textures) noexcept
{
	if (textures[4] == nullptr) return; // Return if just rendering to the backbuffer

	const ShaderProgram& hdrToLDRProgram = *programs[0];

	uint& hdrToLDRFBO = *framebuffers[0];

	const uint& hdrTexture = *textures[0];
	const uint& bloomTexture = *textures[1];
	const uint& gizmosTexture = *textures[2];
	const uint& uiTexture = *textures[3];
	uint& ldrTexture = *textures[4];

	if (hdrToLDRFBO == 0)
	{
		glGenFramebuffers(1, &hdrToLDRFBO);
		glGenTextures(1, &ldrTexture);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, hdrToLDRFBO);

	glBindTexture(GL_TEXTURE_2D, ldrTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, AppInfo::screenWidth, AppInfo::screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ldrTexture, 0);
}
void PostProcess::Defaults::DrawHDRToLDR(
	vector<ShaderProgram*>& programs,
	vector<uint*>& framebuffers,
	vector<uint*>& textures) noexcept
{
	ShaderProgram& hdrToLDRProgram = *programs[0];

	uint& hdrToLDRFBO = *framebuffers[0];

	const uint& hdrTexture = *textures[0];
	const uint& bloomTexture = *textures[1];
	const uint& gizmosTexture = *textures[2];
	const uint& uiTexture = *textures[3];
	uint& ldrTexture = *textures[4];

	glViewport(0, 0, AppInfo::screenWidth, AppInfo::screenHeight);

	// If not intentionally targetting the back buffer initialise a new one
	if (textures[4] != nullptr && hdrToLDRFBO == 0)
	{
		glGenFramebuffers(1, &hdrToLDRFBO);
		glGenTextures(1, &ldrTexture);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, hdrToLDRFBO);

	glClearColor(0.8f, 0.75f, 0.85f, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	hdrToLDRProgram.Bind();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hdrTexture);
	hdrToLDRProgram.BindUniform("HDRTexture", 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bloomTexture);
	hdrToLDRProgram.BindUniform("BloomTexture", 1);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gizmosTexture);
	hdrToLDRProgram.BindUniform("GizmosTexture", 2);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, uiTexture);
	hdrToLDRProgram.BindUniform("UITexture", 3);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, ldrTexture);
	hdrToLDRProgram.BindUniform("CurrentColourBuffer", 4);
	hdrToLDRProgram.BindUniform("Exposure", exposure);
	hdrToLDRProgram.BindUniform("DisplayUI", (int)displayUI);

	screenQuad->Draw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostProcess::Defaults::RefreshFXAA(
	vector<ShaderProgram*>& programs,
	vector<uint*>& framebuffers,
	vector<uint*>& textures) noexcept
{
	if (textures[1] == nullptr) return; // Return if just rendering to the backbuffer

	const ShaderProgram& fxaaProgram = *programs[0];

	uint& fxaaFBO = *framebuffers[0];

	const uint& inputTexture = *textures[0];
	uint& outputTexture = *textures[1];

	if (fxaaFBO == 0)
	{
		glGenFramebuffers(1, &fxaaFBO);
		glGenTextures(1, &outputTexture);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, fxaaFBO);

	glBindTexture(GL_TEXTURE_2D, outputTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, AppInfo::screenWidth, AppInfo::screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, outputTexture, 0);
}
void PostProcess::Defaults::DrawFXAA(
	vector<ShaderProgram*>& programs,
	vector<uint*>& framebuffers,
	vector<uint*>& textures) noexcept
{
	ShaderProgram& fxaaProgram = *programs[0];

	uint& fxaaFBO = *framebuffers[0];

	const uint& inputTexture = *textures[0];
	uint& outputTexture = *textures[1];

	// If not intentionally targetting the back buffer initialise a new one
	if (textures[1] != nullptr && fxaaFBO == 0)
	{
		glGenFramebuffers(1, &fxaaFBO);
		glGenTextures(1, &outputTexture);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, fxaaFBO);

	fxaaProgram.Bind();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, inputTexture);
	fxaaProgram.BindUniform("AliasedTexture", 0);

	glClear(GL_COLOR_BUFFER_BIT);

	screenQuad->Draw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
