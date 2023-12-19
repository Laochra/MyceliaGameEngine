#include "Camera.h"

glm::mat4 Camera::GetViewMatrix()
{
	return glm::mat4();
}

glm::mat4 Camera::GetProjectionMatrix(float w, float h)
{
    return glm::perspective(fov, w / h, nearClip, farClip);
}

void Camera::Update()
{

}