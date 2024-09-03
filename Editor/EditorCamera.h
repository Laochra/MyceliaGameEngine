#pragma once

#include "Camera.h"

#include "Input.h"

class EditorCamera : public Camera
{
public:
	SerialiseAs(EditorCamera);

	using Camera::Camera;

	float xRotation = -90;
	float yRotation = -45;

	float turnSpeed = 25.0f;
	float flySpeed = 2.0f;
	float quickFlySpeed = 6.0f;

	InputAxis xInput;
	InputAxis yInput;
	InputAxis zInput;

	InputBind freeCamera;
	InputBind quickMode;

	glm::mat4 GetViewMatrix() override;

	void Initialise() override;
	void Update() override;
};