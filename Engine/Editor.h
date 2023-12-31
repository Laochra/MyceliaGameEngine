#pragma once

#include "Application.h"

#include <string>

#include "Gizmos.h"

#include "Texture.h"

#include "Shader.h"
#include "Mesh.h"

#include "GameObject3D.h"


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
	Texture meshTexture;

	GameObject3D* object = nullptr;

	struct Light
	{
		glm::vec3 direction;
		glm::vec3 colour;
	};

	Light light = Light();
	glm::vec3 ambientLight = vec3();
};