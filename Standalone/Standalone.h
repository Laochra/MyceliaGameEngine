#pragma once

#include "Application.h"

class Standalone : public Application
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

extern void ScreenResizeCallback(GLFWwindow* window, int width, int height);
