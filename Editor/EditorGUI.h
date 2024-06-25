#pragma once

#include "GUI.h"

typedef unsigned int uint;

namespace EditorGUI
{
	// Scene Buffers
	extern uint sceneViewFrameBufferHDR;
	extern uint sceneViewColourBufferHDR[2];
	extern uint sceneViewColourBufferGizmos;
	extern uint sceneViewDepthStencilBuffer;
	extern uint sceneViewFrameBufferOutput;
	extern uint sceneViewColourBufferOutput;

	extern uint guidFBO;
	extern uint guidDepth;
	extern uint guidTexture;

	extern map<string, uint> windowIndices;

	class EditorWindow
	{
		typedef void (*DrawWindowFunc)(const char* const name, bool& open);

	public:
		string name;
		bool open = false;
		void Draw() noexcept { if (open) drawWindowFunc(name.c_str(), open); }

		EditorWindow(string nameInit, DrawWindowFunc drawFunction) :
			name(nameInit),
			drawWindowFunc(drawFunction)
		{
			windowIndices.insert(pair<string, uint>(nameInit, (uint)windowIndices.size()));
		}
		EditorWindow& operator=(const EditorWindow& other)
		{
			name = other.name;
			open = other.open;
			drawWindowFunc = other.drawWindowFunc;

			return *this;
		}
	private:
		DrawWindowFunc drawWindowFunc;
	};
	extern vector<EditorWindow> editorWindows;

	extern EditorWindow& GetWindow(string name) noexcept;

	extern void Initialise() noexcept;
	extern bool CleanUp() noexcept;
	extern void Draw() noexcept;
	extern void RenderGUI() noexcept;

	extern void FileDropCallback(GLFWwindow* window, int count, const char** filepaths) noexcept;
}