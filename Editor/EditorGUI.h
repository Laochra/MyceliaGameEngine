#pragma once

#include "GUI.h"

namespace EditorGUI
{
	// Scene Buffers
	extern unsigned int sceneViewFrameBufferHDR;
	extern unsigned int sceneViewColourBufferHDR[2];
	extern unsigned int sceneViewColourBufferGizmos;
	extern unsigned int sceneViewDepthStencilBuffer;
	extern unsigned int sceneViewFrameBufferOutput;
	extern unsigned int sceneViewColourBufferOutput;

	// Editor Windows
	extern bool sceneViewOpen;
	extern bool consoleOpen;
	extern bool heirarchyOpen;
	extern bool inspectorOpen;
	extern bool shaderProgramEditorOpen;
	extern bool materialEditorOpen;
	extern bool cameraSettingsOpen;
	extern bool editorStylerOpen;
	extern bool postProcessingOpen;

	extern void Initialise() noexcept;
	extern void CleanUp() noexcept;
	extern void ReadyImGui() noexcept;
	extern void Draw() noexcept;
	extern void RenderGUI() noexcept;
}