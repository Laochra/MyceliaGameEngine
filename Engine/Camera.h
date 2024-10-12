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


	virtual glm::mat4 UpdateProjectionMatrix(float w, float h);
	virtual glm::mat4 UpdateProjectionMatrix();
	virtual glm::mat4 UpdateViewMatrix();
	virtual glm::mat4 UpdatePVMatrix();

	const glm::mat4& GetProjectionMatrix() const noexcept;
	const glm::mat4& GetViewMatrix() const noexcept;
	const glm::mat4& GetPVMatrix() const noexcept;

	void Initialise() override;
	void Update() override;

	virtual void DrawDebug() override;
	
protected:
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;
	glm::mat4 pvMatrix;
};
