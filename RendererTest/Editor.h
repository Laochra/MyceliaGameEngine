#pragma once

#include "Application.h"

#include "Gizmos.h"

class Editor : public Application
{
public:
	Editor() noexcept { }

protected:
	virtual void Initialise() override;
	virtual void FixedUpdate() override;
	virtual void Update() override;
	virtual void Draw() override;
	virtual void OnClose() override;

private:

};