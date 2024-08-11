#pragma once

#include "Shader.h"

typedef unsigned int uint;

class PostProcess
{
public:
	string name;

	vector<ShaderProgram*> programs;
	vector<uint*> framebuffers;
	vector<uint*> textures;

	typedef void (*Func) (
		vector<ShaderProgram*>& programsInit,
		vector<uint*>& framebuffersInit,
		vector<uint*>& texturesInit
	) noexcept;

	PostProcess(
		PostProcess::Func drawFunction,
		PostProcess::Func screenSizeRefreshFunc,
		vector<ShaderProgram*> programsInit,
		vector<uint*> framebuffersInit,
		vector<uint*> texturesInit,
		string name = "Unnamed PostProcess") noexcept;
	PostProcess(const PostProcess&& init) noexcept;

	void Draw() noexcept;
	void RefreshForScreenSize() noexcept;

	// Example PostProcesses
	class Defaults
	{
	public:
		static const int downResFactor = 4;
		static const int bloomPasses = 4;
		// Programs: 0(Ping FBO), 1(Pong FBO)
		// Framebuffers: 0(Ping FBO), 1(Pong FBO)
		// Textures: 0(Input Texture), 1(Ping Texture), 2(Pong Texture)
		static void RefreshBloom(
			vector<ShaderProgram*>& programs,
			vector<uint*>& framebuffers,
			vector<uint*>& textures) noexcept;
		// Programs: 0(Ping FBO), 1(Pong FBO)
		// Framebuffers: 0(Ping FBO), 1(Pong FBO)
		// Textures: 0(Input Texture), 1(Ping Texture), 2(Pong Texture)
		static void DrawBloom(
			vector<ShaderProgram*>& programs,
			vector<uint*>& framebuffers,
			vector<uint*>& textures) noexcept;

		static float exposure;
		static bool displayUI;
		// Programs: 0(HDR To LDR Program)
		// Framebuffers: 0(HDR To LDR FBO)
		// Textures: 0(HDR Texture), 1(Bloom Texture), 2(Gizmos Texture), 3(UI Texture), 4(Output Texture)
		static void RefreshHDRToLDR(
			vector<ShaderProgram*>& programs,
			vector<uint*>& framebuffers,
			vector<uint*>& textures) noexcept;
		// Programs: 0(HDR To LDR Program)
		// Framebuffers: 0(HDR To LDR FBO)
		// Textures: 0(HDR Texture), 1(Bloom Texture), 2(Gizmos Texture), 3(UI Texture), 4(Output Texture)
		static void DrawHDRToLDR(
			vector<ShaderProgram*>& programs,
			vector<uint*>& framebuffers,
			vector<uint*>& textures) noexcept;

		// Programs: 0(FXAA Program)
		// Framebuffers: 0(FXAA FBO)
		// Textures: 0(Input Texture), 1(Output Texture)
		static void RefreshFXAA(
			vector<ShaderProgram*>& programs,
			vector<uint*>& framebuffers,
			vector<uint*>& textures) noexcept;
		// Programs: 0(FXAA Program)
		// Framebuffers: 0(FXAA FBO)
		// Textures: 0(Input Texture), 1(Output Texture)
		static void DrawFXAA(
			vector<ShaderProgram*>& programs,
			vector<uint*>& framebuffers,
			vector<uint*>& textures) noexcept;
	};

protected:
	friend class PostProcessingStack;


	PostProcess::Func drawFunc;
	PostProcess::Func screenSizeRefreshFunc;
	bool usesScreenSize;
};

class PostProcessStack
{
public:
	vector<PostProcess> stack;
	static vector<PostProcess> postProcesses;

	void Add(PostProcess&& postProcess);

	void Draw() noexcept;
	static void RefreshForScreenSize() noexcept;
};
