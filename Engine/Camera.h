#pragma once

#include "GameObject3D.h"

#include "MathIncludes.h"

class Camera : public GameObject3D
{
public:
	using GameObject3D::GameObject3D;
	/// <returns>The GameObject type name as a char string</returns>
	virtual const char* GetName() noexcept override { return "Camera"; }

	float fov = 0;
	float nearClip = 0.1f;
	float farClip = 1000;


	virtual glm::mat4 GetViewMatrix();
	virtual glm::mat4 GetProjectionMatrix(float w, float h);

	void Initialise() override;
};

extern Camera* mainCamera;
extern int screenWidth;
extern int screenHeight;