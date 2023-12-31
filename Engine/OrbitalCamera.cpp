#include "OrbitalCamera.h"

#include "TimeManager.h"
#include "Input.h"

glm::mat4 OrbitalCamera::GetViewMatrix()
{
    vec3 up = vec3(0, 1, 0);
    return glm::lookAt(position, centre, up);
}

void OrbitalCamera::Update()
{
    vec3 forward = glm::normalize(vec3(position.x, 0, position.z) - vec3(centre.x, 0, centre.z));
    vec3 up = vec3(0, 1, 0);
    vec3 right = glm::cross(forward, up);

	KeyAxis xInput(A, D);
	KeyAxis yInput(LShift, Space);
	KeyAxis zInput(S, W);

	// Keyboard Movement
	if (xInput != 0.0f)
	{
		position += -(float)xInput * right * 2.0f * Time::delta;
		centre += -(float)xInput * right * 2.0f * Time::delta;
	}
	if (yInput != 0.0f)
	{
		position += (float)yInput * up * Time::delta;
		centre += (float)yInput * up * Time::delta;
	}
	if (zInput != 0.0f)
	{
		position += -(float)zInput * forward * 2.0f * Time::delta;
		centre += -(float)zInput * forward * 2.0f * Time::delta;
	}

	Keybind cursorToggle(LAlt);

	// Mouse Orbitting
	if (!cursorToggle)
	{
		position += input->cursorMovement.x * right * 2.0f * Time::delta;


		vec3 zoom = input->cursorMovement.y * forward * 2.0f * Time::delta;

		bool minZoomOverflow = glm::distance(vec3(position.x + zoom.x, 0, position.z + zoom.z), vec3(centre.x, 0, centre.z)) < minZoom;
		bool maxZoomOverflow = glm::distance(vec3(position.x, 0, position.z), vec3(centre.x, 0, centre.z)) > maxZoom;

		if ((!minZoomOverflow || input->cursorMovement.y > 0) && (!maxZoomOverflow || input->cursorMovement.y < 0))
		{
			position += zoom;
			position += input->cursorMovement.y * up * 0.75f * Time::delta;
		}
	}
}