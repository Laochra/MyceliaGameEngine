#pragma once

#include "glm.hpp"
#include "ext.hpp"

using glm::vec3;
using glm::mat4;

class Camera
{
public:
	glm::mat4 GetViewMatrix();

	glm::mat4 GetProjectionMatrix(float w, float h);

	Camera()
	{
		fov = glm::pi<float>() * 0.25f;
	}

	void Update();

private:
	vec3 position = vec3(0, 10, 10);
	float theta = -90;
	float phi = -45;

	float fov;

	float nearClip = 0.1f;
	float farClip = 1000;
};