#pragma once

#include "glm.hpp"
#include "ext.hpp"

using glm::vec3;
using glm::mat4;

class Camera
{
public:
	vec3 position = vec3(0, 10, 10);

	float fov;

	float nearClip = 0.1f;
	float farClip = 1000;

	virtual glm::mat4 GetViewMatrix();

	virtual glm::mat4 GetProjectionMatrix(float w, float h);

	Camera()
	{
		fov = glm::pi<float>() * 0.25f;
	}

	virtual void Update();
};