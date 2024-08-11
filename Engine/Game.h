#pragma once

#include "GameObjectManager.h"

class Game // Abstract
{
public:
	bool displayUI = true;

	// Required for editor contexts to be able to display transform gizmos
	// at the correct stage in the render loop.
	GameObjectManager::DrawFunc transformsDrawFunc = nullptr;
	
	// This function is called after the graphics context is created,
	// at the start of the application.
	// Leave renderTarget as nullptr to target the backbuffer when drawing.
	virtual void Initialise(uint* renderTargetInit = nullptr) = 0;
	// This function is called just before the graphics context is destroyed,
	// at the end of the application.
	virtual void OnClose() = 0;

	// This function is called just after Initialise() in a standalone context,
	// or when the play button is pressed in an editor context.
	virtual void OnStart() = 0;
	// This function is called just before OnClose() in a standalone context,
	// or when the stop button is pressed in an editor context.
	// This is the final chance to save and clean up important game data.
	virtual void OnStop() = 0;

	// This function is only called in an editor context when the pause button is pressed,
	// although a game could repurpose it for events like a pause menu or the game losing focus.
	virtual void OnPause() = 0;
	// This function is only called in an editor context when the play button is pressed while paused,
	// although a game could repurpose it for events like a pause menu or the game regaining focus.
	virtual void OnUnpause() = 0;

	// This function is called 60 times every second. That means that if framerate is lower than 60hz,
	// this function will be called additional times each frame to maintain 60 times per second,
	// making it ideal for a physics step or other non-linear events.
	// When in an editor context this only runs in play mode.
	virtual void FixedUpdate() = 0;
	// This function is called once per frame and is ideal for global game logic.
	// When in an editor context this only runs in play mode.
	virtual void Update() = 0;
	// This function is called once per frame and requires the returning of
	// an output texture for the application to render.
	// When in an editor context this will still run every frame.
	virtual void Draw() = 0;
};
