#include "Camera.h"

#include "Time.h"
#include "Input.h"

glm::mat4 Camera::GetViewMatrix()
{
    float thetaR = glm::radians(theta);
    float phiR = glm::radians(phi);

    vec3 forward = vec3(cos(phiR) * cos(thetaR), sin(phiR), cos(phiR) * sin(thetaR));
	vec3 up = vec3(0, 1, 0);

    return glm::lookAt(position, position + forward, up);
}

glm::mat4 Camera::GetProjectionMatrix(float w, float h)
{
    return glm::perspective(fov, w / h, nearClip, farClip);
}

void Camera::Update()
{
	float thetaR = glm::radians(theta);
	float phiR = glm::radians(phi);

	vec3 forward = vec3(cos(phiR) * cos(thetaR), sin(phiR), cos(phiR) * sin(thetaR));
	vec3 right = vec3(-sin(thetaR), 0, cos(thetaR));
	vec3 up = vec3(0, 1, 0);

	if (input->GetKeyDown(A) && !input->GetKeyDown(D))
	{
		position += -right * 2.0f * Time::delta;
	}
	else if (input->GetKeyDown(D) && !input->GetKeyDown(A))
	{
		position += right * 2.0f * Time::delta;
	}

	if (input->GetKeyDown(W) && !input->GetKeyDown(S))
	{
		position += forward * 2.0f * Time::delta;
	}
	else if (input->GetKeyDown(S) && !input->GetKeyDown(W))
	{
		position += -forward * 2.0f * Time::delta;
	}

	if (input->GetKeyDown(Space) && !input->GetKeyDown(LShift))
	{
		position += up * Time::delta;
	}
	else if (input->GetKeyDown(LShift) && !input->GetKeyDown(Space))
	{
		position += -up * Time::delta;
	}


	// Mouse Look
	theta += input->cursorMovement.x * 20 * Time::delta;
	phi += -input->cursorMovement.y * 20 * Time::delta;
	if (phi > 80) phi = 80;
	if (phi < -80) phi = -80;
}