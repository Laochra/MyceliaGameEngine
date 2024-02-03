#pragma once

#include "Camera.h"

#include "Input.h"

class EditorCamera : public Camera
{
public:
	using Camera::Camera;
	/// <returns>The GameObject type name as a char string</returns>
	virtual const char* GetName() noexcept override { return "EditorCamera"; }

	float xRotation = -90;
	float yRotation = -45;

	float flySpeed = 2.0f;
	float quickFlySpeed = 6.0f;

	KeyAxis xInput;
	KeyAxis yInput;
	KeyAxis zInput;

	Keybind freeCamera;
	Keybind quickMode;


	glm::mat4 GetViewMatrix() override;

	void Initialise() override;
	void Update() override;
};