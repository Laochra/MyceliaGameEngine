#pragma once

#include "GameObject3D.h"

#include "MathIncludes.h"

class Camera : public GameObject3D
{
public:
	using GameObject3D::GameObject3D;

	float fov;
	float nearClip = 0.1f;
	float farClip = 1000;


	virtual glm::mat4 GetViewMatrix();
	virtual glm::mat4 GetProjectionMatrix(float w, float h);

	void Initialise() override;
};