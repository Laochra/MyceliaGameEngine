#pragma once

#include "Camera.h"

#include "Input.h"

class LevelEditorCamera : public Camera
{
public:
	SerialiseAs(LevelEditorCamera);

	using Camera::Camera;

	glm::mat4 GetViewMatrix() override;
	glm::mat4 GetProjectionMatrix(float w, float h) override;

	void Initialise() override;
};