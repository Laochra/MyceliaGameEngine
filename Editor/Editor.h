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
};
