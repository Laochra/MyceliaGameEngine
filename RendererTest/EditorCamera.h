#pragma once

#include "Camera.h"

#include "Input.h"

class EditorCamera : public Camera
{
public:
	float xRotation = -90;
	float yRotation = -45;

	float flySpeed = 2.0f;
	float quickFlySpeed = 6.0f;

	KeyAxis xInput;
	KeyAxis yInput;
	KeyAxis zInput;

	Keybind freeCamera;
	Keybind quickMode;

	EditorCamera()
	{
		fov = glm::radians(80.0f);

		xInput.BindPair(A, D);
		yInput.BindPair(LControl, Space);
		zInput.BindPair(S, W);

		freeCamera.Bind(MouseRight);
		quickMode.Bind(LShift);
	}

	glm::mat4 GetViewMatrix() override;

	void Update() override;
};