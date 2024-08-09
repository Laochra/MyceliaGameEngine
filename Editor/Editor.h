#pragma once

#include "Application.h"

#include "LinkedHexGrid.h"
#include "RadialMenu.h"

inline HexType currentTileType = HexType::Grass;

class Editor : public Application
{
public:
	using Application::Application;

protected:
	virtual void Initialise() override;
	virtual void FixedUpdate() override;
	virtual void Update() override;
	virtual void Draw() override;
	virtual bool OnClose() override;

private:
	LinkedHexGrid* hexGrid;
	uint hexPosFBO;
	uint hexPosTexture;
	uint hexPosDepth;

	RadialMenu* radialMenu;
};
