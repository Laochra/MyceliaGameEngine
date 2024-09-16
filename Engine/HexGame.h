#pragma once

#include "Game.h"
#include "Input.h"

#include "PostProcessing.h"

#include "RadialMenu.h"
#include "UISprite.h"

#include "LinkedHexGrid.h"

typedef unsigned int uint;

inline uint currentRadialPage = 0U;
inline vector<TileData>* currentTileType = &HexTile::trees;
inline uint currentTileVariant = 0U;

class HexGame : public Game
{
public:
	struct Inputs
	{
		InputBind place = InputBind({ InputCode::Space, InputCode::GamepadA });
		InputBind openRadial = InputBind({ InputCode::R, InputCode::Q, InputCode::E, InputCode::GamepadX, InputCode::GamepadLB, InputCode::GamepadRB });
		InputAxis moveX = InputAxis(InputCode::A, InputCode::D, InputCode::GamepadLSX);
		InputAxis moveZ = InputAxis(InputCode::W, InputCode::S, InputCode::GamepadLSY);

		InputBind radialSelect = InputBind({ InputCode::Space, InputCode::GamepadA });
		InputBind radialClose = InputBind({ InputCode::Esc, InputCode::R, InputCode::GamepadB, InputCode::GamepadX });
		InputBind radialPageLeft = InputBind({ InputCode::Q, InputCode::GamepadLB });
		InputBind radialPageRight = InputBind({ InputCode::E, InputCode::GamepadRB });
		InputAxis radialX = InputAxis(InputCode::A, InputCode::D, InputCode::GamepadLSX);
		InputAxis radialY = InputAxis(InputCode::W, InputCode::S, InputCode::GamepadLSY);
	};
	HexGame::Inputs gameInputs;

	float moveSpeed = 3.0f;

	LinkedHexGrid* hexGrid;
	RadialMenu* treeRadial;
	RadialMenu* flowerRadial;
	RadialMenu* waterRadial;
	RadialMenu* landRadial;

	UISprite* crosshair;

	struct RenderHandles
	{
		uint hdrFBO = 0;
		uint hdrTexture = 0;
		uint brightTexture = 0;
		uint hdrDepth = 0;
		uint gizmosTexture = 0;

		uint uiFBO = 0;
		uint uiTexture = 0;
		uint uiDepth = 0;

		uint hexPosFBO;
		uint hexPosTexture;
		uint hexPosDepth;

		uint bloomFBOs[2] = { 0, 0 };
		uint bloomTextures[2] = { 0, 0 };

		uint hdrToLDRFBO = 0;
		uint ldrTexture = 0;

		uint outputFBO = 0;
	};
	RenderHandles handles;

	PostProcessStack postProcessStack;

	// This function is called after the graphics context is created,
	// at the start of the application.
	// Leave renderTarget as nullptr to target the backbuffer when drawing.
	void Initialise(uint* renderTargetInit = nullptr);
	// This function is called just before the graphics context is destroyed,
	// at the end of the application.
	void OnClose();

	// This function is called just after Initialise() in a standalone context,
	// or when the play button is pressed in an editor context.
	void OnStart();
	// This function is called just before OnClose() in a standalone context,
	// or when the stop button is pressed in an editor context.
	// This is the final chance to save and clean up important game data.
	void OnStop();

	// This function is only called in an editor context when the pause button is pressed,
	// although a game could repurpose it for events like a pause menu or the game losing focus.
	void OnPause();
	// This function is only called in an editor context when the play button is pressed while paused,
	// although a game could repurpose it for events like a pause menu or the game regaining focus.
	void OnUnpause();

	// This function is called 60 times every second. That means that if framerate is lower than 60hz,
	// this function will be called additional times each frame to maintain 60 times per second,
	// making it ideal for a physics step or other non-linear events.
	// When in an editor context this only runs in play mode.
	void FixedUpdate();
	// This function is called once per frame and is ideal for global game logic.
	// When in an editor context this only runs in play mode.
	void Update();
	// This function is called once per frame and requires the returning of
	// an output texture for the application to render.
	// When in an editor context this will still run every frame.
	void Draw();

private:
	void LoadTileData() noexcept;
	void SaveTileData() noexcept;
};
