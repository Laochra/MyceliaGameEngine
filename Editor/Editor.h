#pragma once

#include "Application.h"

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
};
