#pragma once

#include "GameObject3D.h"

#include "MathIncludes.h"

class Camera : public GameObject3D
{
public:
	SerialiseAs(Camera);

	using GameObject3D::GameObject3D;

	float fov = 0;
	float nearClip = 0.01f;
	float farClip = 1000;


	virtual glm::mat4 GetViewMatrix();
	virtual glm::mat4 GetProjectionMatrix(float w, float h);
	glm::mat4 GetProjectionMatrix();
	glm::mat4 GetPVMatrix();

	void Initialise() override;

	virtual void DrawDebug() override;
};
