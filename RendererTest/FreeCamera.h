#pragma once

#include "Camera.h"

class FreeCamera : public Camera
{
public:
	float theta = -90;
	float phi = -45;

	glm::mat4 GetViewMatrix() override;

	void Update() override;
};