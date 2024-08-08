#pragma once

#include "Application.h"

#include <string>

#include "InputEditor.h"

#include "Texture.h"

#include "Shader.h"
#include "Mesh.h"

#include "GameObject3D.h"
#include "MeshRenderer.h"
#include "LightingManager.h"

#include "GameObjectManager.h"

#include "LinkedHexGrid.h"

#include "RadialMenu.h"

inline HexType currentTileType = HexType::Grass;

class Editor : public Application
{
public:
	Editor() noexcept { }

protected:
	virtual void Initialise() override;
	virtual void FixedUpdate() override;
	virtual void Update() override;
	virtual void Draw() override;
	virtual bool OnClose() override;

private:
	// Post Processing
	ShaderProgram hdrProgram;
	ShaderProgram blurProgram;
	ShaderProgram fxaaProgram;

	LinkedHexGrid* hexGrid;
	uint hexPosFBO;
	uint hexPosTexture;
	uint hexPosDepth;

	RadialMenu* radialMenu;
};
