#include "Camera.h"

glm::mat4 Camera::GetViewMatrix()
{
	return glm::mat4();
}

glm::mat4 Camera::GetProjectionMatrix(float w, float h)
{
	float aspect;
	if (h == 0.0f || w == 0.0f) aspect = 0.0f;
	else aspect = w / h;

    return glm::perspective(fov, aspect, nearClip, farClip);
}

void Camera::Initialise()
{
	fov = glm::radians(80.0f);
}