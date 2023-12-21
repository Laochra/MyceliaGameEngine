#include "EditorCamera.h"

#include "Time.h"

glm::mat4 EditorCamera::GetViewMatrix()
{
    float thetaR = glm::radians(xRotation);
    float phiR = glm::radians(yRotation);

    vec3 forward(cos(phiR) * cos(thetaR), sin(phiR), cos(phiR) * sin(thetaR));
    vec3 up(0, 1, 0);

    return glm::lookAt(position, position + forward, up);
}

void EditorCamera::Update()
{
	float thetaR = glm::radians(xRotation);
	float phiR = glm::radians(yRotation);

	vec3 forward = glm::normalize(vec3(cos(phiR) * cos(thetaR), 0, cos(phiR) * sin(thetaR)));
	vec3 right(-sin(thetaR), 0, cos(thetaR));
	vec3 up(0, 1, 0);

	if (freeCameraToggle)
	{
		// Keyboard Movement
		vec3 xzMovement(0, 0, 0);

		if (glm::abs((float)xInput) > 0.15f)
		{
			xzMovement += (float)xInput * right;
		}
		if (glm::abs((float)zInput) > 0.15f)
		{
			xzMovement += (float)zInput * forward;
		}
		if (xzMovement != vec3(0, 0, 0))
		{
			position += glm::normalize(xzMovement) * (quickMode ? quickFlySpeed : flySpeed) * Time::delta;
		}

		if (yInput != 0.0f)
		{
			position += (float)yInput * up * (quickMode ? quickFlySpeed : flySpeed) * Time::delta;
		}

		// Mouse Look
		xRotation += input->cursorMovement.x * 20 * Time::delta;
		yRotation += -input->cursorMovement.y * 20 * Time::delta;
		if (yRotation > 80) yRotation = 80;
		if (yRotation < -80) yRotation = -80;
	}
}