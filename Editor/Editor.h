#pragma once

#include "Application.h"

#include <string>

#include "InputEditor.h"

#include "Gizmos.h"

#include "Texture.h"

#include "Shader.h"
#include "Mesh.h"

#include "GameObject3D.h"
#include "MeshRenderer.h"
#include "LightingManager.h"

#include "GameObjectManager.h"

class Editor : public Application
{
public:
	Editor() noexcept { }

protected:
	virtual void Initialise() override;
	virtual void OnFrameStart() override;
	virtual void FixedUpdate() override;
	virtual void Update() override;
	virtual void Draw() override;
	virtual void DrawPostProcess() override;
	virtual void DrawGUI() override;
	virtual void OnClose() override;

private:
	MeshRenderer* object = nullptr;

	// Scene View Buffers
	unsigned int sceneViewFrameBufferHDR = 0;
	unsigned int sceneViewColourBufferHDR[2] = { 0, 0 };
	unsigned int sceneViewColourBufferGizmos = 0;
	unsigned int sceneViewDepthStencilBuffer = 0;

	// Post Processing
	ShaderProgram hdrProgram;
	ShaderProgram blurProgram;
	ShaderProgram fxaaProgram;
	unsigned int sceneViewFrameBufferOutput = 0;
	unsigned int sceneViewColourBufferOutput = 0;
	float exposure = 1.0f;

	// Windows
	bool consoleOpen = true;
	bool heirarchyOpen = true;
	bool inspectorOpen = true;
	bool sceneViewOpen = true;
	bool shaderProgramEditorOpen = true;
	bool materialEditorOpen = true;
	bool cameraSettingsOpen = true;
	bool editorStylerOpen = true;
	bool postProcessingOpen = true;
};