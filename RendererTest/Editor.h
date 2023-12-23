#pragma once

#include "Application.h"

#include <string>

#include "Gizmos.h"

#include "Shader.h"
#include "Mesh.h"

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
	virtual void OnClose() override;

private:
	ShaderProgram shader;
	Mesh mesh;
	glm::mat4 quadTransform;
};