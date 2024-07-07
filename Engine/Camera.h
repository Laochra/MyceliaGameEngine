#pragma once

#include "GameObject3D.h"

#include "MathIncludes.h"

class Camera : public GameObject3D
{
public:
	SerialiseAs(Camera);

	using GameObject3D::GameObject3D;

	static Camera* main;

	float fov = 0;
	float nearClip = 0.1f;
	float farClip = 1000;


	virtual glm::mat4 GetViewMatrix();
	virtual glm::mat4 GetProjectionMatrix(float w, float h);
	glm::mat4 GetPVMatrix();

	void Initialise() override;
};

extern int screenWidth;
extern int screenHeight;
extern bool screenSizeJustChanged;