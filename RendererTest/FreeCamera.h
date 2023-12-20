#pragma once

#include "Camera.h"

class FreeCamera : public Camera
{
public:
	float theta = -90;
	float phi = -45;

	float flySpeed = 2.0f;
	float quickFlyMultiplier = 3.0f;

	glm::mat4 GetViewMatrix() override;

	void Update() override;
};