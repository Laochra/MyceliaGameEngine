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
	virtual void DrawGUI() override;
	virtual void OnClose() override;

private:
	MeshRenderer* object = nullptr;

	// Scene View Buffers
	unsigned int sceneViewFrameBuffer = 0;
	unsigned int sceneViewColourBuffer = 0;
	unsigned int sceneViewDepthStencilBuffer = 0;

	// Windows
	bool consoleOpen = true;
	bool heirarchyOpen = true;
	bool inspectorOpen = true;
	bool sceneViewOpen = true;
	bool shaderProgramEditorOpen = true;
	bool cameraSettingsOpen = true;
};