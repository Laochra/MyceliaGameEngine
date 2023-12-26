#pragma once

#include "Application.h"

#include <string>

#include "Gizmos.h"

#include "Texture.h"

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
	glm::mat4 meshTransform;
	Texture meshTexture;

	struct Light
	{
		glm::vec3 direction;
		glm::vec3 colour;
	};

	Light light;
	glm::vec3 ambientLight;
};