#include "FreeCamera.h"

#include "Time.h"
#include "Input.h"

glm::mat4 FreeCamera::GetViewMatrix()
{
    float thetaR = glm::radians(theta);
    float phiR = glm::radians(phi);

    vec3 forward = vec3(cos(phiR) * cos(thetaR), sin(phiR), cos(phiR) * sin(thetaR));
    vec3 up = vec3(0, 1, 0);

    return glm::lookAt(position, position + forward, up);
}

void FreeCamera::Update()
{
	float thetaR = glm::radians(theta);
	float phiR = glm::radians(phi);

	vec3 forward = vec3(cos(phiR) * cos(thetaR), sin(phiR), cos(phiR) * sin(thetaR));
	vec3 right = vec3(-sin(thetaR), 0, cos(thetaR));
	vec3 up = vec3(0, 1, 0);

	KeyAxis xInput(A, D);
	KeyAxis yInput(LShift, Space);
	KeyAxis zInput(S, W);

	// Keyboard Movement
	if (xInput != 0.0f)
	{
		position += (float)xInput * right * 2.0f * Time::delta;
	}
	if (yInput != 0.0f)
	{
		position += (float)yInput * up * Time::delta;
	}
	if (zInput != 0.0f)
	{
		position += (float)zInput * forward * 2.0f * Time::delta;
	}

	Keybind cursorToggle(LAlt);

	// Mouse Look
	if (!cursorToggle)
	{
		theta += input->cursorMovement.x * 20 * Time::delta;
		phi += -input->cursorMovement.y * 20 * Time::delta;
		if (phi > 80) phi = 80;
		if (phi < -80) phi = -80;
	}
}