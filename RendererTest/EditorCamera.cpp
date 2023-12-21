#include "EditorCamera.h"

#include "Time.h"
#include "Input.h"

glm::mat4 EditorCamera::GetViewMatrix()
{
    float thetaR = glm::radians(theta);
    float phiR = glm::radians(phi);

    vec3 forward(cos(phiR) * cos(thetaR), sin(phiR), cos(phiR) * sin(thetaR));
    vec3 up(0, 1, 0);

    return glm::lookAt(position, position + forward, up);
}

void EditorCamera::Update()
{
	float thetaR = glm::radians(theta);
	float phiR = glm::radians(phi);

	vec3 forward = glm::normalize(vec3(cos(phiR) * cos(thetaR), 0, cos(phiR) * sin(thetaR)));
	vec3 right(-sin(thetaR), 0, cos(thetaR));
	vec3 up(0, 1, 0);

	KeyAxis xInput(A, D);
	KeyAxis yInput(LControl, Space);
	KeyAxis zInput(S, W);

	Keybind freeCameraToggle(MouseRight);
	Keybind quickMode(LShift);

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
			position += glm::normalize(xzMovement) * flySpeed * (quickMode ? quickFlyMultiplier : 1.0f) * Time::delta;
		}

		if (yInput != 0.0f)
		{
			position += (float)yInput * up * flySpeed * (quickMode ? quickFlyMultiplier : 1.0f) * Time::delta;
		}

		// Mouse Look
		theta += input->cursorMovement.x * 20 * Time::delta;
		phi += -input->cursorMovement.y * 20 * Time::delta;
		if (phi > 80) phi = 80;
		if (phi < -80) phi = -80;
	}
}