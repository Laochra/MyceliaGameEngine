#pragma once

#pragma once

#include "Camera.h"

class OrbitalCamera : public Camera
{
public:
	using Camera::Camera;

	vec3 centre = vec3(0, 0, 0);

	float minZoom = 2;
	float maxZoom = 30;

	float minElevation = 1;
	float maxElevation = 15;


	glm::mat4 GetViewMatrix() override;

	void Update() override;
};