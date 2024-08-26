#pragma once

#include "Application.h"

#include "LinkedHexGrid.h"

class Editor : public Application
{
public:
	using Application::Application;
	virtual void OnStateChange(AppState newState) override;

protected:
	virtual void Initialise() override;
	virtual void FixedUpdate() override;
	virtual void Update() override;
	virtual void Draw() override;
	virtual bool OnClose() override;
};
